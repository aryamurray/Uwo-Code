#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iterator>
#include <math.h>

#define NODE_RADIUS 25

struct Point
{
    double x;
    double y;
};
struct Node : Point
{
    bool hasHandle1, hasHandle2;
    Point handle1;
    Point handle2;
};

// Initialize our Vector of Nodes so it's accesable globally
std::vector<Node> nodes;
Node *selectedNode = nullptr;
Point *selectedControlPoint = nullptr;

// Function to calculate the Euclidean distance between two points (x1, y1) and (x2, y2)
double distance(double x1, double y1, double x2, double y2)
{
    return std::sqrt(std::pow((x2 - x1), 2) + std::pow((y2 - y1), 2));
}

// Callback function for mouse button events
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    double xpos, ypos;
    int width, height;
    glfwGetCursorPos(window, &xpos, &ypos);     // Get the current cursor position
    glfwGetWindowSize(window, &width, &height); // Get the window size
    ypos = height - ypos;                       // Convert to bottom-left origin

    // If the left mouse button is pressed or released
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        // If the button is pressed
        if (action == GLFW_PRESS)
        {
            bool nodeSelected = false;

            // Check if a node or control point is under the cursor and select it
            for (int i = 0; i < nodes.size(); i++)
            {
                // If the cursor is over a node
                if (distance(nodes[i].x, nodes[i].y, xpos, ypos) < NODE_RADIUS)
                {
                    selectedNode = &nodes[i]; // Select the node
                    nodeSelected = true;
                    break;
                }
                // If the cursor is over the first control point of a node
                else if (nodes[i].hasHandle1 && distance(nodes[i].handle1.x, nodes[i].handle1.y, xpos, ypos) < NODE_RADIUS)
                {
                    selectedControlPoint = &nodes[i].handle1; // Select the control point
                    nodeSelected = true;
                    break;
                }
                // If the cursor is over the second control point of a node
                else if (nodes[i].hasHandle2 && distance(nodes[i].handle2.x, nodes[i].handle2.y, xpos, ypos) < NODE_RADIUS)
                {
                    selectedControlPoint = &nodes[i].handle2; // Select the control point
                    nodeSelected = true;
                    break;
                }
            }

            // If no node or control point is under the cursor, create a new node
            if (!nodeSelected)
            {
                Node newNode{xpos, ypos};   // Create a new node at the cursor position
                double handleOffset = 50.0; // You can adjust this value as needed

                // Set the control points to be on either side of the node
                newNode.handle1 = {xpos - handleOffset, ypos};
                newNode.handle2 = {xpos + handleOffset, ypos};

                // Indicate that the node has control points
                newNode.hasHandle1 = true;
                newNode.hasHandle2 = true;

                nodes.push_back(newNode); // Add the new node to the list of nodes
            }
        }
        else if (action == GLFW_RELEASE)
        {
            // Deselect the node or control point when the mouse button is released
            selectedNode = nullptr;
            selectedControlPoint = nullptr;
        }
    }
}

// Callback function for cursor position events
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height); // Get the window size
    ypos = height - ypos;                       // Convert to bottom-left origin

    // If the left mouse button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (selectedNode)
        {
            selectedNode->x = xpos; // Update the node's x-coordinate
            selectedNode->y = ypos; // Update the node's y-coordinate with inverted value
        }
        else if (selectedControlPoint)
        {
            selectedControlPoint->x = xpos; // Update the control point's x-coordinate
            selectedControlPoint->y = ypos; // Update the control point's y-coordinate
        }
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        nodes.clear();
    }
}

void render_nodes(GLFWwindow *window, std::vector<Node> nodes)
{

    glPointSize(10.0f);
    glColor3f(0, 0, 0);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_POINTS);
    for (int i = 0; i < nodes.size(); i++)
    {
        glVertex2f(nodes[i].x, nodes[i].y);
    }
    glEnd();
}

// Render the control points
void render_control_points(GLFWwindow *window, std::vector<Node> nodes)
{
    if (nodes.size() == 0)
        return;
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(5.0f); // Set point size to a reasonably large value

    glBegin(GL_POINTS);

    for (int i = 0; i < nodes.size(); i++)
    {
        if (i != 0)
        {
            glVertex2f(nodes[i].handle1.x, nodes[i].handle1.y);
        }
        if (i != nodes.size() - 1)
        {
            glVertex2f(nodes[i].handle2.x, nodes[i].handle2.y);
        }
    }
    glEnd();
}

void render_dotted_lines(GLFWwindow *window, std::vector<Node> nodes)
{
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF); // Set stipple pattern for dotted line

    glBegin(GL_LINES);
    for (int i = 0; i < nodes.size(); i++)
    {
        Node &node = nodes[i];
        if (i != 0 && node.hasHandle1) // Skip the first control point
        {
            glVertex2f(node.x, node.y);
            glVertex2f(node.handle1.x, node.handle1.y);
        }
        if (i != nodes.size() - 1 && node.hasHandle2) // Skip the last control point
        {
            glVertex2f(node.x, node.y);
            glVertex2f(node.handle2.x, node.handle2.y);
        }
    }
    glEnd();

    glDisable(GL_LINE_STIPPLE); // Disable line stipple after drawing dotted lines
}

Point interpolate(const Point &p1, const Point &p2, double t)
{
    return {p1.x + t * (p2.x - p1.x), p1.y + t * (p2.y - p1.y)};
}

Point bezierCurve(const Point &p0, const Point &p1, const Point &p2, const Point &p3, double t)
{
    Point a = interpolate(p0, p1, t);
    Point b = interpolate(p1, p2, t);
    Point c = interpolate(p2, p3, t);
    Point d = interpolate(a, b, t);
    Point e = interpolate(b, c, t);
    return interpolate(d, e, t);
}

// Function to draw a Bezier curve given a set of nodes
void drawBezierCurve(std::vector<Node> nodes)
{
    // If there are less than two nodes, we can't draw a curve
    if (nodes.size() < 2)
        return;

    // Set the number of steps for drawing the curve. More steps result in a smoother curve.
    const int numSteps = 200;

    // Enable anti-aliasing for smoother lines
    glEnable(GL_LINE_SMOOTH);

    // Enable blending and set the blend function for smooth transitions between colors
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the line width
    glLineWidth(2.0f);

    // Start drawing lines
    glBegin(GL_LINE_STRIP);

    // Loop over each pair of nodes
    for (size_t i = 0; i < nodes.size() - 1; ++i)
    {
        // For each pair of nodes, draw the curve between them
        for (int j = 0; j <= numSteps; ++j)
        {
            // Calculate the parameter t for the current step
            double t = static_cast<double>(j) / numSteps;

            // Calculate the point on the curve at parameter t using the Bezier curve formula
            Point p = bezierCurve(nodes[i], nodes[i].handle2, nodes[i + 1].handle1, nodes[i + 1], t);

            // Add the calculated point to the line strip
            glVertex2d(p.x, p.y);
        }
    }

    // End drawing lines
    glEnd();
}

int main(int argc, char *argv[])
{
    int WIDTH = 1920;
    int HEIGHT = 1080;

    if (argc == 3)
    {
        WIDTH = atoi(argv[1]);
        HEIGHT = atoi(argv[2]);
        printf("[Info] Setting window size to be %i by %i", WIDTH, HEIGHT);
    }

    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4); // Set the 4x Sampling
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 3", NULL, NULL);
    if (!window)
    {
        printf("[Error] Failed to create window");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current and define view*/
    glfwMakeContextCurrent(window);
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glViewport(0, 0, WIDTH, HEIGHT);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render_nodes(window, nodes);
        render_control_points(window, nodes);
        render_dotted_lines(window, nodes);
        drawBezierCurve(nodes);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
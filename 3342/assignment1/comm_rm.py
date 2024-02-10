import sys
import tokenize
import io

def remove_comments(src: str) -> str:
    """
    Removes comments and docstrings from Python source code.
    """
    f = io.StringIO(src)
    processed_tokens = []
    last_token = None
    in_assignment = False  # Track if we are within an assignment statement

    for tok in tokenize.generate_tokens(f.readline):
        t_type, t_string, _, _, _ = tok

        if t_type == tokenize.COMMENT:
            continue  # Remove single-line comments

        elif t_type == tokenize.STRING:
            if t_string.startswith('"""') or t_string.startswith("'''"):
                if not in_assignment:  # Only remove if not in an assignment
                    continue

        elif t_type == tokenize.NAME and t_string == '=':
            in_assignment = True

        elif t_type in [tokenize.NL, tokenize.NEWLINE]:  # Check for end of assignment
            in_assignment = False

        processed_tokens.append(tok)
        last_token = tok

    return tokenize.untokenize(processed_tokens)


def main():
    if len(sys.argv) != 2:
        print("Usage: python my_script.py <input_file_path>")
        sys.exit(1)

    input_file_path = sys.argv[1]
    try:
        with open(input_file_path, "r") as input_file:
            original_code = input_file.read()
            cleaned_code = remove_comments(original_code)

        # Save the cleaned code to a new file or use it as needed
        with open("cleaned_script.py", "w") as output_file:
            output_file.write(cleaned_code)
        print(f"Cleaned code saved to cleaned_script.py")
    except FileNotFoundError:
        print(f"File not found: {input_file_path}")

if __name__ == "__main__":
    main()



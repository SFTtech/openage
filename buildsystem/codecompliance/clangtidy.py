# Copyright 2024-2024 the openage authors. See copying.md for legal info.

"""
Checks clang-tidy errors on cpp files
"""

import subprocess
from .cppstyle import filter_file_list
from .util import findfiles


def find_issues(check_files, dirnames):
    """
    Invoke clang-tidy to check C++ files for issues.
    Yields issues found by clang-tidy in real-time.
    """
    # Specify the checks to include
    # 4 checks we focus on
    checks_to_include = [
        'clang-analyzer-*',
        'bugprone-*',
        'concurrency-*',
        'performance-*'
    ]
    # Create the checks string
    checks = ', '.join(checks_to_include)

    # Invocation command
    invocation = ['clang-tidy', f'-checks=-*,{checks}']

    # Use utility functions from util.py and cppstyle.py
    if check_files is not None:
        filenames = list(filter_file_list(check_files, dirnames))
    else:
        filenames = list(filter_file_list(findfiles(dirnames), dirnames))

    if not filenames:
        print("No files to check.")
        return  # No files to check

    for filename in filenames:
        # Run clang-tidy for each file
        print(f"Starting clang-tidy check on file: {filename}")
        try:
            with subprocess.Popen(
                invocation + [filename],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            ) as process:
                # Stream output in real-time
                while True:
                    output = process.stdout.readline()
                    if output:
                        yield ("clang-tidy output", output.strip(), None)
                    elif process.poll() is not None:
                        break

                # Capture remaining errors (if any)
                for error_line in process.stderr:
                    yield ("clang-tidy error", error_line.strip(), None)

        # Handle exception
        except subprocess.SubprocessError as exc:
            yield (
                "clang-tidy error",
                f"An error occurred while running clang-tidy on {filename}: {str(exc)}",
                None
            )

import os
import random
import subprocess
import time

# Define the absolute path to git.exe on Windows
GIT_PATH = r"C:\Program Files\Git\cmd\git.exe"

# Automatically find the directory where this script (and your repository) lives
REPO_DIR = os.path.dirname(os.path.abspath(__file__))

# Pools of realistic commit message templates categorized by type
COMMIT_TEMPLATES = {
    "feat": [
        "Implement core logic for {file}",
        "Add helper functions and utilities in {file}",
        "Extend functionality in {file}",
    ],
    "fix": [
        "Handle edge cases and null checks in {file}",
        "Resolve minor parsing issue in {file}",
        "Fix type consistency in {file}",
    ],
    "refactor": [
        "Clean up structure and formatting in {file}",
        "Optimize data handling routines in {file}",
        "Improve readability and modularity of {file}",
    ],
    "docs": [
        "Update documentation and comments for {file}",
        "Add usage notes to {file}",
    ],
    "chore": [
        "Organize assets and cleanup temporary code in {file}",
        "Update configuration references for {file}",
    ],
}


def get_available_files():
  """Retrieves untracked or modified files from git status within the repo."""
  try:
    result = subprocess.run(
        [GIT_PATH, "status", "--porcelain"],
        cwd=REPO_DIR,  # Ensures it runs inside D:\Codex
        capture_output=True,
        text=True,
        check=True,
    )
    files = []
    for line in result.stdout.splitlines():
      # '??' (untracked) or ' M' (modified)
      if line.startswith("?? ") or line.startswith(" M "):
        file_path = line[3:].strip()
        # Skip system or build folders
        if not any(
            skip in file_path
            for skip in ["venv", "__pycache__", ".git", "node_modules"]
        ):
          files.append(file_path)
    return files
  except subprocess.CalledProcessError as e:
    print(f"Error checking git status: {e}")
    return []


def generate_commit_message(filename):
  """Selects a random commit category and message template for the file."""
  category = random.choice(list(COMMIT_TEMPLATES.keys()))
  template = random.choice(COMMIT_TEMPLATES[category])
  clean_name = filename.split("/")[-1].split("\\")[-1]
  return f"{category}: {template.format(file=clean_name)}"


def main():
  files = get_available_files()

  if not files:
    print("No changes or untracked files available to commit.")
    return

  max_commits_to_make = min(random.randint(1, 3), len(files))
  print(
      f"Found {len(files)} eligible files. Planning"
      f" {max_commits_to_make} commit(s) for this run."
  )

  random.shuffle(files)

  for i in range(max_commits_to_make):
    target_file = files[i]
    commit_msg = generate_commit_message(target_file)

    try:
      # Stage the single file
      subprocess.run([GIT_PATH, "add", target_file], cwd=REPO_DIR, check=True)

      # Commit with the dynamic message
      subprocess.run(
          [GIT_PATH, "commit", "-m", commit_msg], cwd=REPO_DIR, check=True
      )
      print(f"[{i+1}/{max_commits_to_make}] Committed: {commit_msg}")

      if i < max_commits_to_make - 1:
        delay = random.randint(30, 180)
        print(f"Pausing for {delay} seconds before next commit...")
        time.sleep(delay)

    except subprocess.CalledProcessError as e:
      print(f"Git command failed during operation: {e}")
      break

  try:
    print("Pushing updates to remote repository...")
    subprocess.run([GIT_PATH, "push"], cwd=REPO_DIR, check=True)
    print("Successfully pushed changes.")
  except subprocess.CalledProcessError as e:
    print(f"Push failed: {e}")


if __name__ == "__main__":
  main()
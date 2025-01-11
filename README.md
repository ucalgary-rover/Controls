# Setting Up Pre-Commit

To ensure consistent code quality and enforce standards, we use **pre-commit** hooks. Follow the steps below to install and verify it:

## Installation

Install pre-commit using pip:
```bash
pip install pre-commit
```


## Verification

After installation, verify the installation by checking the version:
```bash
pre-commit --version
```

Expected output:
```
pre-commit 4.0.1
```

If you see the expected version, pre-commit is successfully installed and ready to use!

## Using git hooks
Run pre-commit install to install pre-commit into your git hooks
```bash
pre-commit install
```

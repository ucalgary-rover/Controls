# Repo Setup

Please run 
```bash
./setup.sh
```
To run the necessary commands for setting up the repo. This step does not include setting up "make" (steps for that can be seen below). These steps have been tested on Windows, but not on IOS or Linux. There is a step by step below that explains the commands and why they are needed.

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

# Enabling git commit template

git commit template gives a format to fill out when writting a commit message. Run the following command from the root of the repo to allow the template to be active:

```bash
git config commit.template .gitmessage
```


# Setting up "make"

"make" is required to run the make file to simplefy compelation of the program. These instructions include installing choco, as its is one way to install make in a simple manor

## Installing choco

Run the following command in a Admin PowerShell terminal
```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```
Close all terminals to make sure the new ones are able to run choco. if done successfully, running:
```bash
choco
```
should give the version

## Installing "make"

In any terminal, run

```bash
choco install make
```

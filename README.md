# MARTHA 

## What is the MARTHA repo for? 

MARTHA (Miniaturized Avionics for Rapid Testing Handling and Assessment) is our
all-in-one flight computer for data collection and flight stage prediction. 

This repo hosts all the software used exclusively by MARTHA. Any code that is also used by other hardware systems are found within the submodules of this repo such as [Avionics](https://github.com/CURocketEngineering/Avionics). We use the submodules to reduce code duplication as many of our systems rely on the same basic ideas (state machines, data handling, etc.).

## Operations
- Initialize each sensor driver
- Initialize data handlers
- Initialize flight status 
- Request data from each sensor driver 
- Pass sensor data into the data handlers
- Pass the data handlers into the flight status updater

## Workspace Setup

1. Clone the repo
```bash
git clone https://github.com/CURocketEngineering/MARTHA.git
```
2. Setup the submodules 
```bash
git submodule init
git submodule update 
```
3. Install the PlatformIO extension: `platformio.platformio-ide`

## How to contribute

We follow the basic [GitHub Flow](https://docs.github.com/en/get-started/using-github/github-flow#following-github-flow).

- The main branch is protected, so you must create a pull request and have it reviewed by at least one other contributor 


![Martha Logo](assets/MarthaLogo.jpg)

NS3-RDCN README
================================

## OS and software preparation:

We base our experiment environment on Ubuntu 20.04 LTS and highly recommend that you do the same. This streamlines the setup process and avoids unexpected issues cause by incompatible software versions etc. Please make sure that you have Python installed. Also make sure that you have root or sudo permission.

This branch contains the entire ns-3 network simulator (ns-3.33) with ns3-gym (opengym) module.

## Install NS3-RDCN 

1. The first part of the preparation is to clone the repository:

```shell
git clone https://github.com/subin993/ns3rdcn.git
```

2. Next, install all dependencies required by ns-3.

```shell
apt-get install gcc g++ python python3-pip
```

3. Install ZMQ and Protocol Buffers libs:

```shell
sudo apt-get update
apt-get install libzmq5 libzmq3-dev
apt-get install libprotobuf-dev
apt-get install protobuf-compiler
```

4. Install Pytorch.

```shell
pip3 install torch
```

Following guideline of installation in https://pytorch.org

5. Building ns-3

```shell
./waf configure
./waf build
```

6. Install ns3-gym

```shell
pip3 install --user ./src/opengym/model/ns3gym
```

## Setting ns-3 environment
To run all of scenarios (Rdcn_controller_{number of ports}ports_exp_{algorithm}_fct.cc) in the directory of scratch, you can configure your own settings for scenarios (Traffic model, Network load (ON/OFF period), Simulation time, etc.) 

## Running ns-3 environment
To run all of the scenarios, open the terminal and run the command:
```shell
./waf --run scratch/Rdcn_controller_{number of ports}ports_exp_{algorithm}_fct.cc
```


## Running various RDCN scheduling algorithms
In the directory scratch, there are various RDCN scheduling algorithms (e.g., Solstice, RotorNet, kEC, P4-DADS, etc.).

Open a new terminal and run the command:

```shell
cd ./scratch
python3 <name-of-algorithm>.py
```

Contact
================================
Subin Han, Korea University, subin993@korea.ac.kr

Eunsok Lee, Korea University, tinedge@korea.ac.kr


How to reference NS3-RDCN?
================================
Please use the following bibtex:


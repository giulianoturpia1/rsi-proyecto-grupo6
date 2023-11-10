 # Visualizing the Dynamic Behaviour of a TSCH/RPL via LEDs

## **Redes de Sensores Inalambricos** Project - 2023

## Table of Contents

# 1. Project Summary

## 1.1. Introduction

The main idea of the project is to visualize interactively, by utilizing LEDs, a network of wireless sensors that use TSCH/RPL. The goal is to observe the operation of the network at different stages of its life (network creation, message exchange, addressing, among others). The results could be used in an interactive environment to promote the better understanding of concepts, or in future iterations of the RSI course.

# 1.2. Motivation

In an RPL network that uses TSCH, there are certain stages during its lifetime, such as the discovery of the DODAG, controlled message exchange passing through a root node, message transmission based on timeslots (TSCH), among others. These events, while they can last over time, there are  such as discovering the network, there are events with very short associated times, an example being message transmission/reception. TSCH network with Orchestra [1], or the path of a UDP message through the DODAG and how different messages are announced in a network (EB, DIO, DAO).

The goal, then, is to have a system that allows visualizing the operation of a network of this kind using LEDs.

# 2. Repository Installation

Issue the following commands, making sure to have previously installled Contiki-NG(via Docker or natively), and set the Git-Hub credentials correctly:

```
cd ~/contiki-ng && git clone git@github.com:giulianoturpia1/rsi-proyecto-grupo6.git
```

# 3. Mode of Operation

By default, the client and server nodes are going to operate in the role of a regular RPL/TSCH network
Typically speaking, the network can have as many client nodes as needed, but a single server node.
The server node from the start is configured as the root of the DODAG in the RPL network as that is the behaviour
that we want to replicate(coordinator mode). If it is necessary(but not recommended), the root can be changed by issuing
in the Contiki-NG shell of the new root(via serial ports):

+ make login PORT= < port > ; port could be: **/dev/ttyACM0**
+ rpl-set-root 1

This is also going to automatically put it in TSCH coordinator mode.

## 3.1. Command Line Settings

The following command line options are available:
* `MAKE_WITH_ORCHESTRA` - use the Contiki-NG Orchestra scheduler.
* `MAKE_WITH_STORING_ROUTING` - use storing mode of the RPL routing protocol.
* `MAKE_WITH_LINK_BASED_ORCHESTRA` - use the link-based rule of the Orchestra shheduler. This requires that both Orchestra and storing mode routing are enabled.

Use the vaule 1 for "on", 0 for "off". By default all options are "off".

# 4. Examples

As was mentioned previously, the project is to have several resources available to visualize certain behaviour of a TSCH/RPL network. Refering to section [3. Mode of Operation](#3-mode-of-operation), there are available several projects to compile and then load into the desired network of nodes, those being:

+ Visualize the Discovery/Update of the Network 
+ Visualize the TX/RX of Nodes.
+ Visualize the Channel Hopping of Nodes.
+ Visualize TimeSlots and FrameSlots.

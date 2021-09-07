# A Wabato Robot

An interactive sculpture created by Wabato (https://www.facebook.com/public/Wabato-Mad)

## Introduction

This repository contains details about the software and hardware that was integrated to add interactivity to a Wabato sculpture. All the related art work is owned by 'Wabato-Mad'. NoSQL Geeks contributed the tech pieces as part of a joint community project with 'Wabato-Mad'(Andreas Birkner) and the local museum in Krumbach/Germany. The BSD license applies only to the source code within this Github repo and does not impact the licensing of the piece of art itself.

## Move your head!

A `NEMA 17` stepper motor is used to move the head of the robot. The motor is controlled by using a `A4588` driver unit and an `Arduino Uno`.

> BTW: All the development happens on a `Raspberry Pi 400`.

A good tutorial about the basic hardware setup can be found here: https://starthardware.org/arduino-a4988-nema17/.

<img src="https://github.com/artofnosql/wabato-robot/blob/main/img/arduino-a4988-nema17.png?raw=true" width="500px"/>

I created a first Arduino [script](./src/ino/stepper/test.ino) for testing this setup. It peforms the following steps:

1. Move the head slowly right
2. Wait a second
3. Move the head slowly left

## Follow me!

The idea is to use three `HC-SR04` ultra-sonic sensors to estimate the position of an observer of the robot. As soon as the potential position is estimated, the robot should turn its head towards the observer.

These sensors are quite easy to use. They have an ultra-sonic sender and recevier built in. All they need is a 5V power supply and you can then trigger a signal by setting the `TRIG` pin to high. This caues that an ultra-sonic wave is sent via the sender. You can then retrieve the reflected wave's signal via the `ECHO` pin that is attached to the receiver. The distance of an oserver is measured by takig into account that a wave moves with a specific speed within space. 

A first test [script](https://github.com/artofnosql/wabato-robot/blob/main/src/ino/sonar/test.ino) was written to evaluate the signal of a single `HC-SR04` sensor.

The script does the following:

1. Send an ultra-sonic signal out for 10 micro-seconds
2. Measure the duration of receiving this signal
3. Convert the duration into a distance
4. Log the distance and the duration out via the Arduino's serial monitor

Here an example output:

```
Sending sonar signal ...
dur = 3875
dist = 65.875
Sending sonar signal ...
dur = 3977
dist = 67.609
```

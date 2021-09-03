# A Wabato Robot

An interactive sculpture created by Wabato (https://www.facebook.com/public/Wabato-Mad)

## Introduction

This repository contains details about the software and hardware that was integrated to add interactivity to a Wabato sculpture. All the related art work is owned by 'Wabato-Mad'. NoSQL Geeks contributed the tech pieces as part of a joint community project with 'Wabato-Mad'(Andreas Birkner) and the local museum in Krumbach/Germany. The BSD license applies only to the source code within this Github repo and does not impact the licensing of the piece of art itself.

## Move your head!

A `NEMA 17` stepper motor is used to move the head of the robot. The motor is controlled by using a `A4588` driver unit and an `Arduino Uno`.

> BTW: All the development happens on a Raspberri 400.

A good tutorial about the basic hardware setup can be found here: https://starthardware.org/arduino-a4988-nema17/.

<img src="https://github.com/artofnosql/wabato-robot/blob/main/img/arduino-a4988-nema17.png?raw=true" width="500px"/>

I created a first Arduino [script](./src/ino/stepper/test.ino) for testing this setup. It peforms the following steps:

1. Move the head slowly right
2. Wait a second
3. Move the head slowly left

## Follow me!

> TODO

# Horse-Carriage

An Arduino project focusing on data transfer via audio. Completed for ITECH's Computational Design and Digital Fabrication course.

## Concept

This Arduino project takes a satirical stance on the creation of useful devices in order to explore capabilities of physical computing. It consists of two robots, the Horse and the Carriage who must work together in order to draw a user-defined path. Conceptually, this project takes a single robot with a path-planning drawing function and, for no useful reason, decides to separate it into two different robots that are connected via a string and sound.

The Carriage is the brains of the operation, it receives a drawing from the user via a touchpad. It rationalizes a path into a set of movement commands then encodes it into a series of floating point numbers. One by one, the Carriage transmits the commands as audio (one approximately every 3 seconds) to the Horse. The Horse is the wheels of the operation. Using a microphone, it receives the floating point number over audio from the Carriage and decodes it into a movement command, which it promptly executes. The Horse pulls the Carriage along with it via a string and together they traverse the path. All the while, the Carriage drags chalk to mark out the users’ drawing.


## Running

Clone the code to your Arduino sketches folder. Copy the `HCShared` directory to the `libraries` folder in your Arduino sketches folder. Start Arduino IDE and select the `Horse` and `Carriage` sketches. Upload them your Arduinos.

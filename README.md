
# Recording Light Sign Controller
Use an Arduino compatible board to control your home studio "Recording" light sign !

# The project
I have a home studio and wanted to have a "Recording" led sign on my door.

I'm using Ardour as a DAW which offers many ways to switch it on an off, like OSC, MIDI, scripts... I thought I could use some cheap Arduino to trigger the led sign using one of these paths.

I finally came across that incredible XIAO ESP32C3 board which embeds Wifi and Bluetooth and a few IOs. It is Arduino compatible over the counter, so pretty easy to use and nicely supported.

# Hardware installation
I installed a 3D printed recording led sign which I got from [myminifactory.com](https://www.myminifactory.com/object/3d-print-recording-led-sign-87178).

I put a cheap led strip inside I bought on my local hardware store and wired the +12V and GND to the inside of my room, up to my desk.

I connected the 12V to the original led strip power supply (220VAC).

To switch that power supply, I used a Sainsmart Solid State Relay board I had from an old project.
I connected one of the solid state relay to a power socket.

The board is powered by the XIAO board 5V which comes from the USB.
The solid state relay control signal is coming from the D10 pin of the XIAO.

# Software principle
On the software part, the XIAO is connecting to the Wifi of my house and pokes Ardour with OSC frames.

Ardour detects it as a control surface and starts throwing OSC feedback to the XIAO board. One of those feedbacks is set to True whenever the main Record button is armed.

When the XIAO board gets that information, it sets D10 pin state accordingly which then controls the led strip !

# gr-hubsan

Blocks for transmitting and receiving Hubsan X4 controller signals (fly quadcopters with SDR!).

Here's a video of it working: https://www.youtube.com/watch?v=OmwEqlOGV94

I take no responsibility for your quadcopters flying off and damaging people/property - assume that this software will break and operate it somewhere suitable.

## Prereqs

 * gnuradio
 * gr-burst
 * gr-eventstream
 * gr-pyqt
 * a HackRF (for included examples - should be fairly easily to modify for other hardware)

## Usage

This can't currently do the Hubsan pairing procedure, so there's a slightly convoluted process to get started:

 * Turn on Hubsan transmitter and connect quadcopter battery, verify that they have paired and the quad flies.
 * Use osmocom_fft or similar to search between 2.4 and 2.5GHz for the control channel.
 * Edit the freq variable in hubsan_rx.grc appropriately and run it. You should lots of output similar to:
```
******* MESSAGE DEBUG PRINT ********
((roll . 132) (pitch . 130) (yaw . 125) (throttle . 0) (tx_id . 1179779243) (a7105_id . 1746746778))
************************************
```
 * Make note of the tx_id and a7105_id and copy them into hubsan_tx.grc
 * Hold the quad tight, making sure you're clear of the props
 * Run hubsan_tx.grc, then move your joystick through its range in all axes to calibrate (the quad will go a bit crazy).
 * Fly.

## Known issues

 * Latency can be brutal with the default HackRF setup. I've included two patches hackrf-16k-buffer.diff and gr-osmosdr-16k-buffer.diff that tone down the USB buffer size a bit, until something proper is worked out.
 * The joystick source is very hardcoded for the "Logitech WingMan Force 3D" joystick I have, get in touch if you're having trouble modifying it for your own setup. PRs welcome for a more confiurable setup.

## Contact

Miek on freenode IRC, usually in #hackrf
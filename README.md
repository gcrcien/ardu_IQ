# Ardu_IQ
IQ Receiver for sdr++
This is a simple receiver using arduino and a SI5351, basically is just the front end of the SDR.
The DSP is handled by the computer and SDR++, the receiver just provides the baseband I and Q data 
to the soundcard, then it gets digitalized and procesed.

![image](https://github.com/gcrcien/ardu_IQ/assets/126195505/6e5eb93d-9fb7-4a9f-932f-ad2d713c63a7)

The RF part consist of a pair of RF preamps and 2 ballanced mixers made with mostly discrete SMD components.
The preamps provide around 15db per stage, and are made with 2n3904 smd parts, although gain is vcc dependant, but can be fed from 4v to 15v with no issues(so far).

![424579117_10211846408353434_9010256545127310122_n](https://github.com/gcrcien/ardu_IQ/assets/126195505/3d1a80f4-77cc-40b6-af06-e938bcd88f41)

Also they seem to be very linear.

![424591631_10211846408713443_1349287362791695741_n](https://github.com/gcrcien/ardu_IQ/assets/126195505/ec0bc785-d965-4829-8e88-2011b7d7c42a)

The diode ring is a premade part to avoid matching diodes, the BAT15-099R, i think its a bit overkill as the 
datasheet mentions this part is good up to 12ghz, however since these are dirt cheap on aliexpress i bought a few, the BN Ferrite cores are also from aliexpress
they are also dirt cheap so i bought about 80 to get free shipping, these are small and have to be wound by hand, but they are more cost effective that ADE-1 in this project 

![WhatsApp Image 2024-06-25 at 22 37 24](https://github.com/gcrcien/ardu_IQ/assets/126195505/4ac97c57-5f20-4059-b032-afd35be033b6)

![WhatsApp Image 2024-06-25 at 22 23 01](https://github.com/gcrcien/ardu_IQ/assets/126195505/7dda2983-7394-46f5-8db2-39382d1c8664)

After the downconversion the signal needs to be amplified a bit, i used a simple lm358 as baseband amplifier, one for I
and the other side for Q, gain here is about 20dB, but i think this part can be substituted for a better one.

![WhatsApp Image 2024-06-25 at 22 23 16](https://github.com/gcrcien/ardu_IQ/assets/126195505/3a8e6c89-f8df-4765-b46c-51b0332e1d28)

After some experimentation i was able to test a different opamp, the TL072 has great perfomrance for this application and you can see the center of the fft and waterfall show less noise and improved linearity overall

![image](https://github.com/gcrcien/ardu_IQ/assets/126195505/5ad5314e-62d2-42f0-aae8-36521a62b7fb)

You can download all the files for Toner transfer method here in the repo, also there's 2 codes for arduino, one for a simple arduino, lcd 16x2, and another for my own vfo which you can find in my other projects 

I might include Cat control in the future, once i Figure out how to do that :D 

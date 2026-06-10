# Embedded Systems Project Report

This report is split into 6 sections:

- Section 1: Project Summary
- Section 2: Technical description of the group implementation
- Section 3: Evidence of testing of group implementation
- Section 4: Technical description of individual implementation
- Section 5: Evidence of testing of individual implementation
- Section 6: Professional considerations

## SECTION 1: Project Summary

**Group project goals**

In this project the group was set the task of using an ARM LPC1768 microcontroller \[otherwise known as an MBED board\] to create a device capable of playing, recording and storing audio data using the standard C programming . As a team we set ourselves a main goal of working together effectively by maintaining good communication. There were also various project goals that were outlined in the brief that the team attempted to adhere to:

- To digitise analogue signals from a source
- To convert digital data stored on SD card to an audible analogue signal
- Record and save audio on the SD card mounted on the MBED board
- Create a user interface on MBED board capable of:
  - Selecting different user modes using keypad and LCD display
  - Select file and playback
  - Allowing audio to be recorded on press of a key
  - View a list of files and file info stored on the SD card.
- Create a user interface on a PC capable of:
  - View a list of files and file info stored on the SD card.
  - Select file and playback
  - Display intensity graph of audio
  - A file manager capable of carrying out basic functions

**Group project outcomes**

The project overall was very successful as the group was able to divide the workload between the four members in a fair fashion that played to each members' strengths and made sure that we established forms of communication that all group members could access continuously \[we used Facebook Messenger and GitHub\], and we also set up weekly group meetings where we could discuss issues face to face. Hence, I believe that our goal of working effectively together was thoroughly met. The first few weeks of the project were difficult due to the CMSIS documentation being quite vague in some areas, this led to confusion over how best to configure the MBED board and which serial interface should be used for the project. As a result, time management became much more difficult. After overcoming these issues, the project went much smoother as there was now a solid foundation from which the entire group could work off of. As a group our final product hit all of the above goals apart from one, the PC GUI did not display the relevant file information, I believe that we were unable to meet this goal due to the timing issues we experienced and not because of a lack of technical knowledge or skill. The final product was a sleek, easy to use implementation of the project brief that worked really well and accurately hit the main points of the brief.

**Individual project goals**

The individual project was a self-chosen project from a list of possibilities given in the project brief that would extend the functionality of the product that had been made by the group. I chose to undertake individual component 3 \[_IC3_\], which was to create a PC GUI that gives a creative real time audio visualisation of audio being played by the MBED. As an individual I set myself the goal of appropriately balancing my time between the group and individual projects. There were also various goals that were outlined in the brief that I attempted to adhere to, to make sure that the audio is presented in a creative way that has:

- Hue content
- Changing light intensity
- Includes a time-based effect, either pulse or fade

**Individual project outcomes**

The individual project depended heavily on the group project as reasonable and reliable way sending the audio via the serial port was required, therefore there were even more strenuous time constraints as the bulk of the project could not be attempted until this had been accomplished. I believe that the project was successful because the majority of the goals outlined in the brief were met, the GUI was very creative and made use of a large range of hues and light intensities. However, a more impressive GUI could have been made if there was more time provided, as the GUI doesn't offer any time-based effect. Therefore, I would say that my personal goal of working balancing my time efficiently was not met because I did not have enough time to fully complete the individual component.

## SECTION 2: Technical description of the group component implementation

**General Hardware**

This is an inhouse built-in user I/O module holding the MBED board, a 16-key keyboard interfaced by a PCF8574, a 16-character 2-line LCD, a USB connection and an SD card reader placed underneath the LCD display.

<img width="1224" height="582" alt="image" src="https://github.com/user-attachments/assets/c0d6dd31-2194-4688-a88e-ff08824968f4" />

The first thing that we decided to do as a group was to set up the hardware by connecting the Audio module and microSD card to the built-in module using wires. These were wired up connected to the modules to the specific pins that were specified in the pin function diagram below. We made sure to colour code all of our wires to make them easier to work with to keep in line with Tog's principals. We also made sure to keep things as simple as possible, which is one of the embedded system principals.

<img width="1208" height="773" alt="image" src="https://github.com/user-attachments/assets/ed2007ec-2549-4dc9-961d-5ee224f3b5b0" />

**Digitising analogue audio signals using audio module**

The audio module has a built-in ADC which is used to sample the incoming analogue sound. We make sure that the sampling rate is greater than 2x greater than the maximum frequency in order that we get the most accurate sound digitised as we possibly can, we originally chose 8k sampling and did this by every 25clocks of timer increase internal counter by 1 call interrupt after 24 times call interrupt and reset timer for, we did this to keep in line with the results of Harry Nyquist's research. We then needed to decide what type of serial connection we were going to use to send the digital data through the MBED board. At first we attempted to make use of the SPI interface but we encountered huge difficulties in getting the correct configuration, but in order to save time and move along with our project we decided to make use of bit bashing by using the I2C connection to send the control bites on the rising edge of the clock. After this we then sent the 7bit data packets to the relevant address. Then when the data for each bit had been set we would then toggle the other audio channel then repeat this process in order to send stereo data. Below is the register map we made use of when setting the bits. We also considered making use of the I2S connections instead of bit bashing, there would have been considerable advantages of using I2S as I2S conversion occurs as data arrives so no buffers would be needed which would result in smoother audio. However, we decided against using I2S because there would have been a conflict with the SSP pins that were being used by the SD card, which would've mean that we would have had to completely change the SD card mode, so we saved time and used this time to focus on other parts of the project.

<img width="1035" height="364" alt="image" src="https://github.com/user-attachments/assets/68ef5bf1-deba-4be1-8372-086824431469" />

**Storing Digitised audio on microSD card**

To store the digitised data we were receiving we decided that we needed a filesystem for the SD card. We could have used the SD card as an on-board chip where we would record data page by page, this would've been very difficult and time consuming, so we decided against doing this. We then downloaded the FatFs filesystem, which allows for commands such as f_write to write bits of data on a file stored on the microSD card.

<img width="1135" height="547" alt="image" src="https://github.com/user-attachments/assets/da77a22e-a79d-471f-99c2-d8caccc263ea" />

Once the filesystem had been downloaded, as shown above, we only needed to edit the mmc.c and device.h files so that they reflected the actual hardware we were using. After this had been done, we then initialised a large buffer, then we write to the buffer in 4 bit chunks until the buffer is full. Then we make use of interrupts to take the buffer and write it to the end of a file using f_write. After this we go back to the start of the file and create a 44 byte header.

**Playing audio**

To play audio we first read the audio file stored on the microSD card and read the header to see if the file is mono or stereo. We then initialise a buffer and write 4-bit chunks from the file to it until the buffer is full. We then call an interrupt and then send the bits to the audio codec, which has a DAC to then be outputted to a sound system and played.

**Interface on built-in user I/O**

The keypad is a 16-key keyboard interfaced by a PCF8574 mapped in a 4x4 matrix. There are 4 wires in and 4 wires out, all of the bits are stored in a single register. To use the keypad, we needed to set a row high by writing a nibble to it. When a key is pressed a column is set low, so we then read which row was set and which column is low to determine which key had been pressed. Then we mapped these to different symbols. This worked on a GPIO interrupt. We then write data to the LCD by using the I2C bus to create a menu that also worked on GPIO interrupts. At first, we attempted to use polling, which resulted in a smoother menu, however we had to use interrupts as we needed many different threads of our program to run for it to be useable.

**PC GUI**

For the PC GUI we needed to find a way to send data using the serial port, between the PC and the MBED board. The GUI was made using python so we made use of python's asyncio library to allow us to both read and write from the PC to the MBED, we could've made use of threading instead but we decided against this due to already having 2 threads in our main program that we were finding difficult to manage. We then created a list of callback functions that would be passed into another function as an argument when serial data is received. On the MBED we made use of non-blocking reads and writes to speed up the communication, the MBED would read the command buffer and reads the bytes inside if a delimiter is seen then it treats the bytes as a command and either writes out the specific data being requested. To send bits in an audio file we take a 16bit sample of the audio packets then get an average in a window of a chosen size and send that average using the serial port to make sure the rate of communication is good enough.

The GUI is shown below, it was made using tkinter in Python. Every step of the design process we made sure to keep Norman's design principals in mind to make sure a good easy to use GUI was created. We made sure that there was great visibility by using contrasting colours that are easy on the user, we also made sure that the GUI gave feedback as every change would be shown and a cursor would show what was currently selected, in addition we made use of mappings from file names to radio buttons so that the system was easy to understand.

<img width="1131" height="531" alt="image" src="https://github.com/user-attachments/assets/054261f4-8dc7-4a2a-a629-41dd05f00c06" />

The intensity graph above was made using Python's matplotlib library. A graphics library such as pyQT could have been used to provide better visuals, but we decided that we should keep our solution simple as an audio visualiser was my chosen individual project so a creative visualisation would be added anyway.

## SECTION 3: Evidence of Testing of group implementation

**Test Strategy**

Our group aimed to be very thorough when testing as the more testing that was done on our product, the more stable and reliable the product would be. We outlined our tests in a table before we began the project so that we could accurately measure our progress against our goals and make corrections if there were erroneous results from the tests. Furthermore, we made sure that we used a collaborative working environment (GitHub), so that we could carry out a code review before committing changes to our project. Carrying out the code review did mean that the project took a bit longer than expected, but it was worth it as it meant that only high-quality code that we knew worked was used. In addition to this I decided to plan the tests that I would run according to the diagram below:

<img width="284" height="167" alt="image" src="https://github.com/user-attachments/assets/2fade991-fc7c-4a40-9c05-dcaf62e0b3c0" />


I chose to test in the manner shown because the Unit Testing would first check to see if the individual software components/objects are working. Then the integration tests would be testing to see if individual software components are able to work together, and the Requirements testing would allow us to verify that the requirements outlined in the project brief actually work.

All in all, I would say that the group testing has been successful because we have a stable product that is very useable and there are rarely any errors that could cause the system to be totally unusable. Although complete testing of a system is almost impossible due to time and funding constraints, I believe that the tests we have run are thorough and quite comprehensive.

BELOW IS THE TABLE SHOWING THE ACTUAL TESTS THAT WERE CARRIED OUT AND THE RESULTS:
| Test Type            | Test Case Description                                                 | Tests                                                                     | Preconditions &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;                         | Post Conditions &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;                             | Expected Result                                                                   | Actual result                                                                                                                           | Date Executed |
| -------------------- | --------------------------------------------------------------------- | ------------------------------------------------------------------------- | ------------------------------------- | ------------------------------------------- | --------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------- | ------------- |
| Unit Test            | Sending a single character using serial communication from PC to MBED | self.sendCommand("V","a")                                                 | N/A                                   | PC terminal shows 01000001                  | PC terminal showing "a"                                                           | Good result, easy fix required to change from binary to character                                                                       | 26/02/19      |
| Unit Test            | Sending a single character using serial communication from MBED to PC | write_usb_serial_blocking("Deleting...\\n\\r",13)                         | N/A                                   | Python window showing "Deleting\\n"         | Python window showing "Deleting\\n                                                | Good result                                                                                                                             | 26/02/19      |
| Unit Test            | Sending a clock signal along Bclock of SPI                            | SPI_Init(LPC_SPI, &spi_cfg)                                               | Oscilloscope showing background noise | Oscilloscope showing a periodic square wave | Oscilloscope showing a periodic square wave                                       | Good result                                                                                                                             | 7/02/19       |
| Integration Testing  | PC requesting audio data from MBED                                    | CALLBACKS\["A:"\] = lambda \*v: app.printVal()                            | N/A                                   | <img width="468" height="525" alt="image" src="https://github.com/user-attachments/assets/881d7df8-58fb-4bf9-97ad-7f0336e1133b" />| The MBED should send decimal values each with a new line and raw string character | Good result                                                                                                                             | 5/03/19       |
| Integration Testing  | Push values to LED 10 line display                                    | ledCtl(n, audioFrames, recentMax);<br><br>displayVolume(recentMax >> 12); | <img width="468" height="312" alt="image" src="https://github.com/user-attachments/assets/254bfeb4-8808-4624-8fac-fa3ad658cf24" />| <img width="325" height="310" alt="image" src="https://github.com/user-attachments/assets/802078a9-22f0-47d9-a348-60df63243733" />| Change in the LED should turn on                                                  | Good Result                                                                                                                             | 15/03/19      |
| Requirements Testing | Copy a file                                                           | Multiple lines of code                                                  | <img width="486" height="851" alt="image" src="https://github.com/user-attachments/assets/9cdfcc87-85e3-4492-9039-b6e502c2ab84" />|  <img width="468" height="853" alt="image" src="https://github.com/user-attachments/assets/fbe826c1-e527-4cdb-9ecc-abc0e0b0492c" />| Copied file should be added to the list with name "copy.wav'                      | Good result as copying occurs, however no other system functions can be used whilst copying takes place. Copying also takes a long time | 15/03/19      |
| Requirements Testing | Record audio and save a file                                          | Multiple lines of code                                                    | <img width="110" height="313" alt="image" src="https://github.com/user-attachments/assets/2778a63d-b12d-4447-a1bf-c13596878c31" />|<img width="118" height="313" alt="image" src="https://github.com/user-attachments/assets/fdf5bc3a-9400-4fae-a477-5aa7f3f4e953" />| Recorded file should be stored and shown within a list on LCD                     | Bad result. The copied file overwrites another file and file with the given name is empty                                               | 15/03/19      |
| Requirements Testing | Files stored on microSD card                                          | Multiple lines of code                                                    | N/A                                   |  <img width="468" height="567" alt="image" src="https://github.com/user-attachments/assets/92354a25-bbd4-4927-bb39-78ca86b34cc7" />| Stored Files should appear in a list on microSD card                              | Good result                                                                                                                             | 15/03/19      |


## SECTION 4: Technical Description of individual component implementation

**Creating the Visualisation**

Firstly I decided that I wanted to make the visualisation for my GUI before thinking about the sound data, so I decided to use Python and use the pyGraph, pyQt and blender libraries to form a cool graphic representation. I could have made use of many different programming languages to form the GUI such as C++ or Java script, C++ in particular would have been an attractive proposition due to how close it is to C, which is the language the group project was made using, however I stuck to Python because I felt more comfortable with it. I created a class in python and began using the libraries to create 18 geometric cubes that would change height and colour as a result of the incoming serial data. The result of this is shown below.

<img width="534" height="571" alt="image" src="https://github.com/user-attachments/assets/94125458-ec52-4831-9876-c3ca6b4c37b9" />

**Receiving serial data**

After this I started to deal with the audio data. I first needed to establish a way for the GUI to receive serial audio data from the MBED board. Since I was using Python for the graphics I used Python's asyncio library to allow me to both read and write from the PC to the MBED, I could've made use of threading instead but I decided against this due to already having multiple threads in main program of the group component that were difficult to manage. I then created a list of callback functions that would be passed into another function as an argument when serial data is received. On the MBED I made use of non-blocking reads and writes to speed up the communication, the MBED would read the command buffer and reads the bytes inside if a delimiter is seen then it treats the bytes as a command and either writes out the specific data being requested. To send bits in an audio file we take a 16bit sample of the audio packets then get an average in a window of a chosen size and send that average using the serial port to make sure the rate of communication is good enough for real time visualisation to take place.

**Handling audio data**

I decided to make another python program that would not be part of the final solution, but would help inform my design decisions by giving me the range of amplitudes that the PC was receiving from the MBED and the different frequencies so that I could use them to make my audio visualiser reactive. I made use of matplotlib to do this, but using Matlab would have been a better choice due to the integrated graphing, however I felt more comfortable using Python. To handle the audio data I created a 1024 bit buffer called Chunk that would store the incoming audio from the serial bus, then for the length of the audio in seconds run a loop that would analyse each Chunk and place the value in another buffer called frame and then plot the frames against the chunks to see a dynamic amplitude being shown. I then noted the maximum and minimum values and also the median value to use for my calculations later. The sample rate of the MBED was 44.1KB so I could expect to get around 20fps for my dynamic audio visualisation. Although this is okay it could be better to provide an even more spectacular visualisation, the only way I could think of speeding this up is to use a DMA instead of buffers, however I do not believe that a serial port would granted permissions by the PC operating System, rendering this solution impossible.

I also wanted to find common frequency values, which required me to make use of the Fast Fourier Transformation \[FFT\]. The FFT algorithm is a O(n log n) computes the Discrete Fourier Transformation \[DFT\] of a sequence. DFT breaks down the sequence into varying frequencies. I made use of the Cooley-Turkey FFT by using the FFT function defined in the scipi library. It is a divide and conquer algorithm that recursively breaks down a DFT of any size such that N = N<sub>1</sub>N<sub>2</sub> into many smaller DFTs of sizes N1 and N2. I could have attempted to write my own FFT algorithm in Python, however this would have been very slow and would have defeated the purpose of using FFT in the first place. Below is the spectrum of frequencies obtained by using FFT and the amplitude graph.

<img width="1150" height="567" alt="image" src="https://github.com/user-attachments/assets/85732912-169d-45a7-b526-5608add51a2c" />

**Dynamic audio visualisation**

Finally, I made my dynamic GUI by making the 18 cubes each represent a common audio frequency and then I computed the height of the cubes by making them a product to the amplitudes of each frequency that was represented by the 18 cubes. After this I set the colours that I would use to fill each cube to represent different levels of audio, low sound, medium or high. By doing this a dynamic GUI that changed at a rate of 20fps was created, I also tried to make a time-based pulsing effect but I was unsuccessful due to the pulsing causing all the elements on the GUI to go black. Below the GUI is shown at 2 different time points.

<img width="711" height="561" alt="image" src="https://github.com/user-attachments/assets/b29052fe-8bfe-4ffd-a25b-f7b5d19be346" />

## SECTION 5: Evidence of Testing of individual implementation

**Test Strategy**

I aimed to be very thorough when testing as the more testing that was done on my product, the more stable and reliable the product would be. I outlined my tests in a table before I began the project so that we could accurately measure my progress against my goals and make corrections if there were erroneous results from the tests. I made use of static code analysis using the grammatech program before committing any changes to my individual GitHub repository. I used static code analysis because code review was no longer an option as the individual project was to be undertaken without any external help. Carrying out the static code analysis did mean that the project took a bit longer than expected, but it was worth it as it meant that only high-quality code that I knew worked was used. In addition to this I decided to plan the tests that I would run according to the diagram below:

<img width="284" height="167" alt="image" src="https://github.com/user-attachments/assets/2fade991-fc7c-4a40-9c05-dcaf62e0b3c0" />

I chose to test in the manner shown because the Unit Testing would first check to see if the individual software components/objects are working. Then the integration tests would be testing to see if individual software components are able to work together, and the Requirements testing would allow us to verify that the requirements outlined in the project brief actually work.

All in all, I would say that the individual testing has been successful because we have a stable product that is very useable and there are rarely any errors that could cause the system to be totally unusable. Although complete testing of a system is almost impossible due to time and funding constraints, I believe that the tests we have run are thorough and quite comprehensive.

BELOW IS THE TABLE SHOWING THE ACTUAL TESTS THAT WERE CARRIED OUT AND THE RESULTS:
| Test Type            | Test Case Description                                                 | Tests                                             | Preconditions &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  | Post Conditions &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;                          | Expected Result                                                                   | Actual result                                                            | Date Executed |
| -------------------- | --------------------------------------------------------------------- | ------------------------------------------------- | ------------- | --------------------------------------- | --------------------------------------------------------------------------------- | ------------------------------------------------------------------------ | ------------- |
| Unit Test            | Sending a single character using serial communication from PC to MBED | self.sendCommand("V","a")                         | N/A           | PC terminal shows 01000001              | PC terminal showing "a"                                                           | Good result, easy fix required to change from binary to character        | 26/02/19      |
| Unit Test            | Sending a single character using serial communication from MBED to PC | write_usb_serial_blocking("Deleting...\\n\\r",13) | N/A           | Python window showing "Deleting\\n"     | Python window showing "Deleting\\n                                                | Good result                                                              | 26/02/19      |
| Unit Test            | Storing audio data chunks in a buffer                                 | data = stream.read(CHUNK)                         | N/A           | Error message saying "Input overflowed" | Chunks of data bits                                                               | Bad result, must be fixed for project to move ahead successfully         | 7/03/19       |
| Integration Testing  | PC requesting audio data from MBED                                    | CALLBACKS\["A:"\] = lambda \*v: app.printVal()    | N/A           | <img width="468" height="516" alt="image" src="https://github.com/user-attachments/assets/97c6034b-24a7-479d-b5d1-2caa8636b0a5" />| The MBED should send decimal values each with a new line and raw string character | Good result                                                              | 5/03/19       |
| Requirements Testing | Checking that the GUI is dynamic and changes over time                | Multiple lines of code                            |<img width="488" height="569" alt="image" src="https://github.com/user-attachments/assets/b52e9555-c87d-451a-9f24-1d5894a97e07" />|<img width="488" height="569" alt="image" src="https://github.com/user-attachments/assets/1a00d60d-a1c8-46bb-81bd-55b2f8e49343" />| 2 different images taken within half a second of each other                       | Good results                                                             | 5/04/19       |
| Requirements Testing | Checking that the GUI changes hue content and light intensity         | Multiple lines of code                            |<img width="415" height="567" alt="image" src="https://github.com/user-attachments/assets/c5535298-a592-4839-930b-6afe32f83d80" />|<img width="415" height="567" alt="image" src="https://github.com/user-attachments/assets/1dbc58ce-03b3-4b54-a51b-b36bc0ae0edb" />| A GUI that varies largely from what it previously looked like                     | Good result                                                              | 5/04/19       |
| Requirements Testing | Checking that a time based pulse effect works                         | Multiple lines of code                            | N/A           |<img width="491" height="567" alt="image" src="https://github.com/user-attachments/assets/2732ba6b-5c6c-4130-9996-fc89d93a76b1" />| Should have bars pulsing with a grey colour                                | Bad result. Does not work at all and must be removed for product to work | 25/03/19      |


## Section 6: Professional Considerations

**Project SWOT analysis**
| SWOT | Helpful to achieving the objective| Harmful to achieving the objective|                                            
| -------------------- | -----------------------------------------------|-----------------------------------------------| 
| Internal Origin       | -David has great knowledge of C programming<br> -Danny has strong knowledge of electronics<br> -Evie has strong understanding Tkinter<br> -James has good organisational skills<br> -Good communication and team working skills<br> -Mini projects knowledge and code -<br>Good knowledge of oscilloscopes| -No knowledge of audio protocols<br> -Most of us have limited prior experience of embedded systems<br> -Motivational Issues 
| External Origin            | -Support from module leader<br> -Gaining skills in embedded systems design and implementation | -Parallel deadlines<br> -Illness<br> -Destroying hardware<br> -Weather-Strikes<br> -Labs only open 9-5 Monday to Friday|


**Risk Register**
| Issue of Concern | Likelihood | Severity | Mitigation Strategy|                                            
| -------------------- | --------------|------------|-------------------------------------------------------------|
| Lack of time due to parallel deadlines |High| Med| Leave sufficient margins so that missing a day of work won’t set us back too much.| 
| Illness |High| Med/Low | Make sure tasks are well documented. Design the schedule so that there are at least 2 people per task so that the other person can step in if their partner is ill.| 
| Destroying hardware |High| Low | Make sure safety measures specific to said hardware are in place such as using correct resistors. | Weather | Med | High | Leave sufficient margins so that missing a day of work won’t set us back too much.| 
| Hardware being stolen or lost | Med | High | Keep hardware components stored away properly.| Strikes | Low | High | Leave sufficient margins so that missing a day of work won’t set us back too much.|
| Team arguments | Med | Med| Talk the module leader to help mediate these situations. | 

**Team Project schedule with personnel hours shown**

<img width="1760" height="1134" alt="image" src="https://github.com/user-attachments/assets/e0108cdf-ba2f-4038-9861-ac02e040e71f" />


**Discussion of an environmental issue associated with the use of my solution as a product**

When making use of any product we must always consider how the product will be disposed of after it has fully fulfilled its use or no longer works at an optimal level. Considering this, I believe that a huge environmental issue of this product is that most of the parts cannot be recycled, the plastics in the connecting wires, the breadboard as a whole and the majority of the components on the built-in I/O module. Therefore, this waste is likely to end up in landfill, which causes many issues. Firstly, it contributes to global warming as greenhouse gasses are given off when this type of waste it burnt. Also, some of the materials within these components such as nickel are toxic and could cause to the pollution of the wider environment including wildlife. According to the Environmental Protection Agency about 60% of discarded electronics ends up in landfill sites, with devastating effects. One consideration would be to repurpose the electronic parts, the MBED board can be used for such a huge variety of projects that there would never really be a need to throw it away, the wires can be used in different circulatory and most of the components on the built-in I/O module could be reused, with the exception of the LCD which usually has quite a short life cycle.

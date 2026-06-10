## Embedded Systems Project Report

This report is split into 6 sections:

- Section 1: Project Summary
- Section 2: Technical description of the group implementation
- Section 3: Evidence of testing of group implementation
- Section 4: Technical description of individual implementation
- Section 5: Evidence of testing of individual implementation
- Section 6: Professional considerations

# SECTION 1: Project Summary

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

# SECTION 2: Technical description of the group component implementation

**General Hardware**

This is an inhouse built-in user I/O module holding the MBED board, a 16-key keyboard interfaced by a PCF8574, a 16-character 2-line LCD, a USB connection and an SD card reader placed underneath the LCD display.

The first thing that we decided to do as a group was to set up the hardware by connecting the Audio module and microSD card to the built-in module using wires. These were wired up connected to the modules to the specific pins that were specified in the pin function diagram below. We made sure to colour code all of our wires to make them easier to work with to keep in line with Tog's principals. We also made sure to keep things as simple as possible, which is one of the embedded system principals.

**Digitising analogue audio signals using audio module**

The audio module has a built-in ADC which is used to sample the incoming analogue sound. We make sure that the sampling rate is greater than 2x greater than the maximum frequency in order that we get the most accurate sound digitised as we possibly can, we originally chose 8k sampling and did this by every 25clocks of timer increase internal counter by 1 call interrupt after 24 times call interrupt and reset timer for, we did this to keep in line with the results of Harry Nyquist's research. We then needed to decide what type of serial connection we were going to use to send the digital data through the MBED board. At first we attempted to make use of the SPI interface but we encountered huge difficulties in getting the correct configuration, but in order to save time and move along with our project we decided to make use of bit bashing by using the I2C connection to send the control bites on the rising edge of the clock. After this we then sent the 7bit data packets to the relevant address. Then when the data for each bit had been set we would then toggle the other audio channel then repeat this process in order to send stereo data. Below is the register map we made use of when setting the bits. We also considered making use of the I2S connections instead of bit bashing, there would have been considerable advantages of using I2S as I2S conversion occurs as data arrives so no buffers would be needed which would result in smoother audio. However, we decided against using I2S because there would have been a conflict with the SSP pins that were being used by the SD card, which would've mean that we would have had to completely change the SD card mode, so we saved time and used this time to focus on other parts of the project.

**Storing Digitised audio on microSD card**

To store the digitised data we were receiving we decided that we needed a filesystem for the SD card. We could have used the SD card as an on-board chip where we would record data page by page, this would've been very difficult and time consuming, so we decided against doing this. We then downloaded the FatFs filesystem, which allows for commands such as f_write to write bits of data on a file stored on the microSD card.

Once the filesystem had been downloaded, as shown above, we only needed to edit the mmc.c and device.h files so that they reflected the actual hardware we were using. After this had been done, we then initialised a large buffer, then we write to the buffer in 4 bit chunks until the buffer is full. Then we make use of interrupts to take the buffer and write it to the end of a file using f_write. After this we go back to the start of the file and create a 44 byte header.

**Playing audio**

To play audio we first read the audio file stored on the microSD card and read the header to see if the file is mono or stereo. We then initialise a buffer and write 4-bit chunks from the file to it until the buffer is full. We then call an interrupt and then send the bits to the audio codec, which has a DAC to then be outputted to a sound system and played.

**Interface on built-in user I/O**

The keypad is a 16-key keyboard interfaced by a PCF8574 mapped in a 4x4 matrix. There are 4 wires in and 4 wires out, all of the bits are stored in a single register. To use the keypad, we needed to set a row high by writing a nibble to it. When a key is pressed a column is set low, so we then read which row was set and which column is low to determine which key had been pressed. Then we mapped these to different symbols. This worked on a GPIO interrupt. We then write data to the LCD by using the I2C bus to create a menu that also worked on GPIO interrupts. At first, we attempted to use polling, which resulted in a smoother menu, however we had to use interrupts as we needed many different threads of our program to run for it to be useable.

**PC GUI**

For the PC GUI we needed to find a way to send data using the serial port, between the PC and the MBED board. The GUI was made using python so we made use of python's asyncio library to allow us to both read and write from the PC to the MBED, we could've made use of threading instead but we decided against this due to already having 2 threads in our main program that we were finding difficult to manage. We then created a list of callback functions that would be passed into another function as an argument when serial data is received. On the MBED we made use of non-blocking reads and writes to speed up the communication, the MBED would read the command buffer and reads the bytes inside if a delimiter is seen then it treats the bytes as a command and either writes out the specific data being requested. To send bits in an audio file we take a 16bit sample of the audio packets then get an average in a window of a chosen size and send that average using the serial port to make sure the rate of communication is good enough.

The GUI is shown below, it was made using tkinter in Python. Every step of the design process we made sure to keep Norman's design principals in mind to make sure a good easy to use GUI was created. We made sure that there was great visibility by using contrasting colours that are easy on the user, we also made sure that the GUI gave feedback as every change would be shown and a cursor would show what was currently selected, in addition we made use of mappings from file names to radio buttons so that the system was easy to understand.

The intensity graph above was made using Python's matplotlib library. A graphics library such as pyQT could have been used to provide better visuals, but we decided that we should keep our solution simple as an audio visualiser was my chosen individual project so a creative visualisation would be added anyway.

# SECTION 3: Evidence of Testing of group implementation

**Test Strategy**

Our group aimed to be very thorough when testing as the more testing that was done on our product, the more stable and reliable the product would be. We outlined our tests in a table before we began the project so that we could accurately measure our progress against our goals and make corrections if there were erroneous results from the tests. Furthermore, we made sure that we used a collaborative working environment (GitHub), so that we could carry out a code review before committing changes to our project. Carrying out the code review did mean that the project took a bit longer than expected, but it was worth it as it meant that only high-quality code that we knew worked was used. In addition to this I decided to plan the tests that I would run according to the diagram below:

I chose to test in the manner shown because the Unit Testing would first check to see if the individual software components/objects are working. Then the integration tests would be testing to see if individual software components are able to work together, and the Requirements testing would allow us to verify that the requirements outlined in the project brief actually work.

All in all, I would say that the group testing has been successful because we have a stable product that is very useable and there are rarely any errors that could cause the system to be totally unusable. Although complete testing of a system is almost impossible due to time and funding constraints, I believe that the tests we have run are thorough and quite comprehensive.

BELOW IS THE TABLE SHOWING THE ACTUAL TESTS THAT WERE CARRIED OUT AND THE RESULTS:

# SECTION 4: Technical Description of individual component implementation

**Creating the Visualisation**

Firstly I decided that I wanted to make the visualisation for my GUI before thinking about the sound data, so I decided to use Python and use the pyGraph, pyQt and blender libraries to form a cool graphic representation. I could have made use of many different programming languages to form the GUI such as C++ or Java script, C++ in particular would have been an attractive proposition due to how close it is to C, which is the language the group project was made using, however I stuck to Python because I felt more comfortable with it. I created a class in python and began using the libraries to create 18 geometric cubes that would change height and colour as a result of the incoming serial data. The result of this is shown below.

**Receiving serial data**

After this I started to deal with the audio data. I first needed to establish a way for the GUI to receive serial audio data from the MBED board. Since I was using Python for the graphics I used Python's asyncio library to allow me to both read and write from the PC to the MBED, I could've made use of threading instead but I decided against this due to already having multiple threads in main program of the group component that were difficult to manage. I then created a list of callback functions that would be passed into another function as an argument when serial data is received. On the MBED I made use of non-blocking reads and writes to speed up the communication, the MBED would read the command buffer and reads the bytes inside if a delimiter is seen then it treats the bytes as a command and either writes out the specific data being requested. To send bits in an audio file we take a 16bit sample of the audio packets then get an average in a window of a chosen size and send that average using the serial port to make sure the rate of communication is good enough for real time visualisation to take place.

**Handling audio data**

I decided to make another python program that would not be part of the final solution, but would help inform my design decisions by giving me the range of amplitudes that the PC was receiving from the MBED and the different frequencies so that I could use them to make my audio visualiser reactive. I made use of matplotlib to do this, but using Matlab would have been a better choice due to the integrated graphing, however I felt more comfortable using Python. To handle the audio data I created a 1024 bit buffer called Chunk that would store the incoming audio from the serial bus, then for the length of the audio in seconds run a loop that would analyse each Chunk and place the value in another buffer called frame and then plot the frames against the chunks to see a dynamic amplitude being shown. I then noted the maximum and minimum values and also the median value to use for my calculations later. The sample rate of the MBED was 44.1KB so I could expect to get around 20fps for my dynamic audio visualisation. Although this is okay it could be better to provide an even more spectacular visualisation, the only way I could think of speeding this up is to use a DMA instead of buffers, however I do not believe that a serial port would granted permissions by the PC operating System, rendering this solution impossible.

I also wanted to find common frequency values, which required me to make use of the Fast Fourier Transformation \[FFT\]. The FFT algorithm is a O(n log n) computes the Discrete Fourier Transformation \[DFT\] of a sequence. DFT breaks down the sequence into varying frequencies. I made use of the Cooley-Turkey FFT by using the FFT function defined in the scipi library. It is a divide and conquer algorithm that recursively breaks down a DFT of any size such that N = N<sub>1</sub>N<sub>2</sub> into many smaller DFTs of sizes N1 and N2. I could have attempted to write my own FFT algorithm in Python, however this would have been very slow and would have defeated the purpose of using FFT in the first place. Below is the spectrum of frequencies obtained by using FFT and the amplitude graph.

**Dynamic audio visualisation**

Finally, I made my dynamic GUI by making the 18 cubes each represent a common audio frequency and then I computed the height of the cubes by making them a product to the amplitudes of each frequency that was represented by the 18 cubes. After this I set the colours that I would use to fill each cube to represent different levels of audio, low sound, medium or high. By doing this a dynamic GUI that changed at a rate of 20fps was created, I also tried to make a time-based pulsing effect but I was unsuccessful due to the pulsing causing all the elements on the GUI to go black. Below the GUI is shown at 2 different time points.

# SECTION 5: Evidence of Testing of individual implementation

**Test Strategy**

I aimed to be very thorough when testing as the more testing that was done on my product, the more stable and reliable the product would be. I outlined my tests in a table before I began the project so that we could accurately measure my progress against my goals and make corrections if there were erroneous results from the tests. I made use of static code analysis using the grammatech program before committing any changes to my individual GitHub repository. I used static code analysis because code review was no longer an option as the individual project was to be undertaken without any external help. Carrying out the static code analysis did mean that the project took a bit longer than expected, but it was worth it as it meant that only high-quality code that I knew worked was used. In addition to this I decided to plan the tests that I would run according to the diagram below:

I chose to test in the manner shown because the Unit Testing would first check to see if the individual software components/objects are working. Then the integration tests would be testing to see if individual software components are able to work together, and the Requirements testing would allow us to verify that the requirements outlined in the project brief actually work.

All in all, I would say that the individual testing has been successful because we have a stable product that is very useable and there are rarely any errors that could cause the system to be totally unusable. Although complete testing of a system is almost impossible due to time and funding constraints, I believe that the tests we have run are thorough and quite comprehensive.

BELOW IS THE TABLE SHOWING THE ACTUAL TESTS THAT WERE CARRIED OUT AND THE RESULTS:

# Section 6: Professional Considerations

**Project SWOT analysis**

**Risk Register**

**Team Project schedule with personnel hours shown**

**Discussion of an environmental issue associated with the use of my solution as a product**

When making use of any product we must always consider how the product will be disposed of after it has fully fulfilled its use or no longer works at an optimal level. Considering this, I believe that a huge environmental issue of this product is that most of the parts cannot be recycled, the plastics in the connecting wires, the breadboard as a whole and the majority of the components on the built-in I/O module. Therefore, this waste is likely to end up in landfill, which causes many issues. Firstly, it contributes to global warming as greenhouse gasses are given off when this type of waste it burnt. Also, some of the materials within these components such as nickel are toxic and could cause to the pollution of the wider environment including wildlife. According to the Environmental Protection Agency about 60% of discarded electronics ends up in landfill sites, with devastating effects. One consideration would be to repurpose the electronic parts, the MBED board can be used for such a huge variety of projects that there would never really be a need to throw it away, the wires can be used in different circulatory and most of the components on the built-in I/O module could be reused, with the exception of the LCD which usually has quite a short life cycle.

# metaTait
Code base for capstone metatait project


IMAGE PROCESSING POC:

*Must have Matlab
1) Clone project
2) Set Matlab path file to ~/ImageProcessingPOC
3) Set desired resolution (based on cage parameters or debugging preferences).  Width must be a multiple of LEDs/Post.  

  >> height = desired_height_number;
  >> width = desired_width_number;

4) Run program in the Matlab command window with either of the following commands:

  >> MainProgram('ImagesFolderColors', height, width)
  
or
  >> MainProgram('ImagesFolderNumbers', height, width)
  
ImagesFolderColors has 6 images, and ImagesFolderNumbers has 18 images.  If you wanted to make your own image folder, that is also an option.

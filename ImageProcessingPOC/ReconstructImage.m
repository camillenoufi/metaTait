function [ matrix ] = ReconstructImage( packet)
%ReconstructImage takes the data packet representing a 2D image and creates
%   a 3D matrix out of that image.  Two dimensions will correpsond to a
%   resolution W and H of an image, and the 3rd dimension will be the RGB
%   color content of each pixel, ranging from 0 to 255
%
% Input:  packet: contains header, and RGB content, respectively for
% each pixel column cross section of the image
% Output: reconstructedImageMatrix:  a HxWxRGB matrix holding image data
  
header = packet(1:2);
height = header(1);
width = header(2);

endPacket = size(packet);
endPacket = endPacket(2);
packet = packet(3:endPacket);

matrix = zeros(height,width,3,'uint8');


%fix!!!!
for i=1:width
    column = packet(3*height*(i-1)+1:3*height*i);
    for j=1:3
        matrix(:,i,j) = column(height*(j-1)+1:height*j);
    end
end
    
end


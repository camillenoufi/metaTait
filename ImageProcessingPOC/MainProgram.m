function [dataStream] = MainProgram(image_folder,height,width,numImages)

% Assumptions:   Number of images is a multiple of the number of LED strips
%


%check width mod(6)
if mod(width,6) ~= 0
    errordlg('Width must be divisible by number of LEDs per post!');
end

imageStruct = importImages(image_folder,numImages);

fileID = fopen('datapacket.txt','w');   %clear data stream file to be overwritten
fileID2 = fopen('binarypacket.bin','w'); %clear data stream file to be overwritten
fclose(fileID);
fclose(fileID2);

ledNames = ['L1'; 'L2'; 'L3'; 'L4'; 'L5'; 'L6'];   %array representing LEDs on each post
posts = ['a'; 'b'; 'c'];                        %array representing post names

imageArray = zeros(height,width,3,numImages);

figure

for i=1:numImages
    %deal with beginning of folder being non-images.....?
    imageArray(:,:,:,i) = CreateImageMatrix(image_folder,imageStruct(i).name,height,width);
    downsampledImageMatrix = uint8(imageArray(:,:,:,i));
    subplot(6,3,i);
    imshow(downsampledImageMatrix)
    title(['V.A ' num2str(i) ' Adjusted/Downsampled'])

end

% convert 3D matrix into serial data packets
[ dataMatrix, sectionLength, height ] = CreateDataPacket2(imageArray, ledNames);

[dataStream] = CreateDataStream(dataMatrix,height,width,size(posts,1));

SavePacketToFile(dataStream)
fileID = fopen('datapacket.txt','a+');
fprintf(fileID,'EOF');
fclose(fileID);

% read data packet, construct timing simulation
DiscreteTimeReconstruction('binarypacket.bin', height, width);
    
end
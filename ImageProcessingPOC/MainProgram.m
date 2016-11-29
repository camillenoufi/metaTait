function [] = MainProgram(image_folder,height,width)

imageStruct = dir(image_folder);

if imageStruct(1).name == '.DS_Store'           % remove struct file headers
    imageStruct = imageStruct(4:end);
else imageStruct = imageStruct(4:end);
end

structLength = size(imageStruct,1);

fileID = fopen('datapacket.txt','w');   %clear datapacket file to be overwritten
fclose(fileID);

ledNames = ['L1'; 'L2'; 'L3'; 'L4'; 'L5'; 'L6'];   %array representing LEDs on each post
postNames = ['a'; 'b'; 'c'];                        %array representing post names

imageArray = zeros(height,width,3,structLength);

for i=1:structLength
    %deal with beginning of folder being non-images.....?
    imageArray(:,:,:,i) = CreateImageMatrix(image_folder,imageStruct(i).name,height,width);
end

% convert 3D matrix into serial data packets
[ dataPacket, sectionLength, height ] = CreateDataPacket2(imageArray, ledNames);

for i = 1:size(dataPacket,1)
    SavePacketToFile(dataPacket(i,:))
end

% read data packet, construct timing images, and display post "image"
ConstructPostImages(dataPacket,structLength, sectionLength, height);

% figure
% imshow(reconstructedImageMatrix)
% title('Reconstructed Matrix')


% compare reconstructed and original downsampled matrix to confirm match
% boolMatch = CompareMatrices(reconstructedImageMatrix, downsampledImageMatrix);

% if match, save to file, else print error message
% if boolMatch
%     Disp('matrices match, saving image data to file...');

%         outputMessage = 'data saved';
%     else outputMessage = 'save failed';
%     end    
% else 
%     Disp('matrices do not match');
% end





fileID = fopen('datapacket.txt','a+');
fprintf(fileID,'EOF');
fclose(fileID);
    
end
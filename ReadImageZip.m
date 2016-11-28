function [] = ReadImageZip(image_folder,height,width)

imageStruct = dir(image_folder);
imageStruct = imageStruct(4:end);
structLength = size(imageStruct,1);

fileID = fopen('datapacket.txt','w');
fclose(fileID);

Ln = ['L1' 'L2' 'L3' 'L4' 'L5' 'L6'];   %array representing LEDs on each post
Pn = ['a' 'b' 'c'];   %array representing post names
imageArray = zeros(height,width,3,structLength);

for i=1:structLength
    %deal with beginning of folder being non-images.....?
    imageArray(:,:,:,i) = CreateImageMatrix(image_folder,imageStruct(i).name,height,width);
end

% convert 3D matrix into serial data packet
imagePacket = CreateDataPacket2(imageArray, Ln);

% read data packet, reconstruct, and display image
%reconstructedImageMatrix = ReconstructImage(imagePacket);

% figure
% imshow(reconstructedImageMatrix)
% title('Reconstructed Matrix')


% compare reconstructed and original downsampled matrix to confirm match
% boolMatch = CompareMatrices(reconstructedImageMatrix, downsampledImageMatrix);

% if match, save to file, else print error message
% if boolMatch
%     Disp('matrices match, saving image data to file...');
SavePacketToFile(imagePacket)
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
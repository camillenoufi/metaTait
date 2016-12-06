function [] = MainProgram(image_folder,height,width)

%check width mod(6)
if mod(width,6) ~= 0
    errordlg('Width must be divisible by number of LEDs per post!');
end

imageStruct = dir(image_folder);
 
% if imageStruct(1).name == '.DS_Store'           % remove struct file headers
%     imageStruct = imageStruct(4:end);
% else
imageStruct = imageStruct(3:end);


structLength = size(imageStruct,1);

fileID = fopen('datapacket.txt','w');   %clear datapacket file to be overwritten
fclose(fileID);

ledNames = ['L1'; 'L2'; 'L3'; 'L4'; 'L5'; 'L6'];   %array representing LEDs on each post
postNames = ['a'; 'b'; 'c'];                        %array representing post names

imageArray = zeros(height,width,3,structLength);
figure

for i=1:structLength
    %deal with beginning of folder being non-images.....?
    imageArray(:,:,:,i) = CreateImageMatrix(image_folder,imageStruct(i).name,height,width);
    downsampledImageMatrix = uint8(imageArray(:,:,:,i));
    subplot(6,3,i);
    imshow(downsampledImageMatrix)
    title(['V.A ' num2str(i) ' Adjusted/Downsampled'])

end

% convert 3D matrix into serial data packets
[ dataPacket, sectionLength, height ] = CreateDataPacket2(imageArray, ledNames);

for i = 1:size(dataPacket,1)
    SavePacketToFile(dataPacket(i,:))
end

t = cputime;
% read data packet, construct timing images, and display post "image"
CalcDiscreteTimePostData(dataPacket,structLength, sectionLength, height);
msResult = cputime-t;

fileID = fopen('datapacket.txt','a+');
fprintf(fileID,'EOF');
fclose(fileID);
    
end
function [  ] = DiscreteTimeReconstruction( binary_file,image_height, image_width )
%%%% reconstruct images from data packet in a similar fashion to how the
%%%% microcontroller will read the packet during one rotation of the cage.

%%%% TO USE OVER TIME: place a breakpoint at the line just under the "updates"
%%%% for-loop.  Each iteration will update the output figure of the n images to display the
%%%% created pixels during that update

%%%% Calculations Revelant to Simulation only are:  ViewingAngle, Column,
%%%%      and Row Index


fileData = fopen(binary_file);
data_stream = fread(fileData);

%set up data parameters
streamLength = size(data_stream,1);
chunkSize = 3;
rgbDim = 3;
imageHeight = image_height*rgbDim;

byte = 1;        %in C/MCU code, byte incrememnts have to be +/- 8 instead of 1?
numImages = 18;
stripsPerPort = 2;
totalStrips = 6;

numStrips = totalStrips/stripsPerPort;
height = imageHeight*stripsPerPort*byte;
updates = image_width*chunkSize;             
updateLength = streamLength/updates;   %also = height*numStrips
columnLength = updates/numImages;      %also = imageWidth/totalStrips

%initialize viewing Angle and image matrix
imArray = uint8(ones(image_height,image_width,rgbDim,numImages))*255;
imArraySize = size(imArray);
viewingAngleL1 = 1;
column = 1;

figure


%parse data stream. Here, each byte has length 1
for u = 1:updates
    index = updateLength*(u-byte) + 1;                          % increment to correct update start in stream packet
    
    %set extraction bounds for each strip
    startL1L2 = index;
    endL1L2 = index+height-byte;
    startL3L4 = index + height;
    endL3L4 = index + 2*height - byte;
    startL5L6 = index + 2*height;
    endL5L6 = index+3*height - byte;
    
    L1L2 = data_stream(startL1L2 : endL1L2);                 % extract corresponding data for LED strip pairs
    L3L4 = data_stream(startL3L4 : endL3L4);
    L5L6 = data_stream(startL5L6 : endL5L6);
    
    %In MCU Code:  send out start packet (32-bits each) to each strip group (1-3) here
    
    %reset pixel index for every update
    pixel = 0;
    %iterate through each RGB pixel in each strip group
    for p = 1:rgbDim*byte:height                 
        pixel = pixel + 1;
        rowIndex = GetRowIndex(pixel,imageHeight/3);    %remove RGB consideration
        
        group = 1;
        L1L2blueByte = L1L2(p);             %get blue byte of pixel       
        L1L2greenByte = L1L2(p+byte);       %get red byte of pixel
        L1L2redByte = L1L2(p+2*byte);       %get green byte of pixel
        [viewingAngle1, columnIndex1] = GetViewingAngleAndColumn(p,group,viewingAngleL1,numImages,height,column,columnLength);
        imArray(rowIndex,columnIndex1,1,viewingAngle1) = L1L2redByte;
        imArray(rowIndex,columnIndex1,2,viewingAngle1) = L1L2greenByte;
        imArray(rowIndex,columnIndex1,3,viewingAngle1) = L1L2blueByte;
        
        group = 2;
        L3L4blueByte = L3L4(p);             %get blue byte of pixel       
        L3L4greenByte = L3L4(p+byte);       %get red byte of pixel
        L3L4redByte = L3L4(p+2*byte);       %get green byte of pixel
        [viewingAngle2, columnIndex2] = GetViewingAngleAndColumn(p,group,viewingAngleL1,numImages,height,column,columnLength);
        % send out L1 pixel start/global brightness (8-bits)
        imArray(rowIndex,columnIndex2,1,viewingAngle2) = L3L4redByte;
        imArray(rowIndex,columnIndex2,2,viewingAngle2) = L3L4greenByte;
        imArray(rowIndex,columnIndex2,3,viewingAngle2) = L3L4blueByte;
        
        group = 3;
        L5L6blueByte = L5L6(p);             %get blue byte of pixel       
        L5L6greenByte = L5L6(p+byte);       %get red byte of pixel
        L5L6redByte = L5L6(p+2*byte);       %get green byte of pixel
        [viewingAngle3, columnIndex3] = GetViewingAngleAndColumn(p,group,viewingAngleL1,numImages,height,column,columnLength);
        imArray(rowIndex,columnIndex3,1,viewingAngle3) = L5L6redByte;
        imArray(rowIndex,columnIndex3,2,viewingAngle3) = L5L6greenByte;
        imArray(rowIndex,columnIndex3,3,viewingAngle3) = L5L6blueByte;
        
        %uncomment loop to see pixel by pixel display
%         for i=1:numImages
%             subplot(5,4,i)
%             imshow(imArray(:,:,:,i))
%             title(['Image ' num2str(i) ' Reconstructed'])
%             drawnow
%         end

    end
    
    %increment column
    column = mod(column,columnLength) + 1;
    
    %increment L1's viewing angle
    if column == 1
        viewingAngleL1 = viewingAngleL1 + 1;
    end
    
    %update output display
    for i=1:numImages
        subplot(5,4,i)
        imshow(imArray(:,:,:,i))
        title(['Image ' num2str(i) ' Reconstructed'])
        drawnow
    end
    
    subplot(5,4,20)
    plot(u,u,'xr','markersize',10)
    title('Current Update')
    drawnow
    
end
    
end




function [viewing_angle, column] = GetViewingAngleAndColumn(p,group,viewing_angle,num_images,height,column, column_length)
    
    %move to  strip-indicated section of image (3, or 5)
    switch group
        case 2
            viewing_angle = mod(viewing_angle + 2,num_images);           %move from L1->L3
            column = column + column_length*2;
        case 3
             viewing_angle = mod(viewing_angle + 4,num_images);          %moving from L1->L5
             column = column + column_length*4;
    end
    
    %determine if column section should be 2, 4, or 6
    if p > height/2
        viewing_angle = mod(viewing_angle + 1,num_images);       %moving from L1->L2 for example
        column = column + column_length;
    end

    if viewing_angle == 0            %check for mod = 0 case
        viewing_angle = num_images;
    end 
    
end



function [rowIndex] = GetRowIndex(pixel,image_height)
    rowIndex = mod(pixel,image_height);              % in matlab, first index is bottom pixel in L->R image
        if rowIndex == 0;
            rowIndex = image_height;
        end
        rowIndex = (image_height+1)-rowIndex;     
end


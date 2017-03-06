function [ dataStream ] = CreateDataStream(dataMatrix,imageHeight,imageWidth,chunkSize)

numUpdates = imageWidth*chunkSize;
numLeds = 6;
rgbDim = 3;

dataStream = [-1,-1];   %initialize array with header

for u = 1:numUpdates    %for every update an led will go through in 1 revolution
    startPixel = imageHeight*rgbDim*(u-1)+1;
    endPixel = imageHeight*rgbDim*u;
    for l = 1:numLeds   % for every led strip
        columnData = dataMatrix(l,startPixel:endPixel);
        dataStream = horzcat(dataStream, columnData); 
    end   
end

dataStream = uint8(dataStream(3:end));      %remove header and convert to unsigned bytes

end


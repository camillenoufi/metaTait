function [ postImagesArray ] = ConstructPostImages(data_packet,num_images, column_num, height)

numLEDs = size(data_packet,1);
rgbDim = 3;
postImagesArray = [];

figure 
for n = 1:num_images

    %extract time_n sections of packet
    tnData = data_packet(:,(n-1)*height*rgbDim*column_num + 1 :n*height*rgbDim*column_num);

    rData = zeros(height,1);
    gData = zeros(height,1);
    bData = zeros(height,1);

    for i = 1:numLEDs
        for j = 1:column_num

            %calculate indices
            startingIndexRed = (j-1)*height*3 + 1;
            endingIndexRed = (j-1)*height*3 + height;
            startingIndexGreen = startingIndexRed + height;
            endingIndexGreen = endingIndexRed + height;
            startingIndexBlue = startingIndexRed + 2*height;
            endingIndexBlue = endingIndexRed + 2*height;

            %extract columns and concatenate into color matrix
            rData = horzcat(rData, transpose(tnData(i, startingIndexRed : endingIndexRed )));
            gData = horzcat(gData, transpose(tnData(i, startingIndexGreen : endingIndexGreen )));
            bData = horzcat(bData, transpose(tnData(i, startingIndexBlue : endingIndexBlue )));
        end
    end

    %remove initialization column
    rData = rData(:,2:end);
    gData = gData(:,2:end);
    bData = bData(:,2:end);

    %create 3D matrix for RGB "image"
    t0ImageMatrixDouble = cat(3,rData,gData,bData);

    t0ImageMatrixInt8 = uint8(t0ImageMatrixDouble);
    
    subplot(3,6,n);
    imshow(t0ImageMatrixInt8)
    title(['t' num2str(n) ' image'])
    
    postImagesArray = cat(4, postImagesArray, t0ImageMatrixInt8);
    
end



end


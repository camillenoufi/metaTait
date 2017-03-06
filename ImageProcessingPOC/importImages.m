function [ imageStruct ] = importImages(image_folder,numImages)


%build structure of images to use as viewing angles
imageStruct = dir(image_folder);
structLength = size(imageStruct,1);   %should be equal to va

if structLength ~= numImages
    
    %in most Macs
    if strcmp(imageStruct(1).name,'.DS_Store')        % remove struct file headers
        imageStruct = imageStruct(4:end);
    end
    
    imageStruct = imageStruct(3:numImages+2);   % remove added files by computer file manager
    % set breakpoint here if you are unsure if your computer will add extra
    % files, and examine imageStruct
    structLength = size(imageStruct,1);   %should be equal to va
    if structLength ~= numImages
        errordlg('imageStruct contains extra files added by File Manager, set breakpoints and  adjust accordingly');
    end
end


end


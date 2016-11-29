function [ ] = SavePacketToFile( packetLine )
%SavePacketToFile saves a data packet to an output file on the datapath
%
% Inputs: packet:  
% Output: boolSuccess:  a boolean value (1 or 0) that indicates if (1) the
%                     save was successful or (0) if they it was not

fileID = fopen('datapacket.txt','a+');
fprintf(fileID,'%02x,',packetLine);             %saves comma deliminated text file to datapacket.txt
fprintf(fileID,'\n');
fclose(fileID);


end


function [ ] = SavePacketToFile( dataStream )
%SavePacketToFile saves a data packet to an output file on the datapath

fileID = fopen('datapacket.txt','a+');          %append file
fprintf(fileID,'%02x,',dataStream);             %save comma deliminated text file to datapacket.txt
fclose(fileID);


fileID2 = fopen('binarypacket.bin','a');        %append file    
fwrite(fileID2,dataStream);                     %saves binary output to binarypacket.bin

end


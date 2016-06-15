# protocol buffers

### Intro

* usually the message communication involve exchanging the data between one or more machines. The data may contain variables of different types and are to be kept in the same fashion at the sender that the receiver understands. Meaning a common understanding to be known between the two or more parties.



* The information is then represented in the structured format that defines the placement and types of data that the structure can contain. The structure is then transmitted over the network.


* However, serialisation comes into the picture when we are transmitting data between two different machines of different architectures (such as big and little endian). Thus we convert everything that we transmit into the network endian (usually big endian) and transmit and the receiver converts it back to host endian (its machine endian format) and reads the data after. Then, it comes to the encoding of types and data, for ex, the `long` type is of 4 bytes on a 32 bit machine, however in the most common present world we are using almost all 64 bit machines. However, the exchange of communication is also possible with the embedded devices such as phones, GPS receivers, routers, switches etc. The exact representation of the data is to be defined in this situation. This is where the serialisation comes into the picture.


* Serialsing the data involves telling the other party the type of the each element in the structure that we are sending, the size each element can hold to etc.


* One of the serialisation formats is the protocol buffer format.


* The protocol buffers asks to define the structure that we wanted to send in a `.proto` format.


* For example:

        package position_info;
        
        enum {
            Mode_No_fix = 0;
            Mode_2D_fix = 1;
            Mode_3D_fix = 2;
        } ModeOfFix;
    
        message position_information {
            ModeOfFix fix_mode = 1 [default = Mode_No_fix];
            double latitude = 2;
            double longitude = 3;
            double altitude = 4;
        }
        
        
The above example defines a fix mode, latitude, longitude and altitude in a structure position_information. The position_information is what we wanted to send over the network. The position_information may be filled from the GPS data. See that the protobuf allows us to send the double types over the network. The .proto file starts with a package definition to avoid naming conflicts between different packages or different structure defintions in different files.
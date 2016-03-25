#ifndef OPCODES_H
#define OPCODES_H
// List of hex values and discriptions of Opcodes

#define OpPoll             0x2000  // This is an ArtPoll packet, no other data is contained in this UDP packet
#define OpPollReply        0x2100  // This is an ArtPollReply Packet. It contains device status information.
#define OpDiagData         0x2300  // Diagnostics and data logging packet.
#define OpCommand          0x2400  // Used to send text based parameter commands.
#define OpOutput           0x5000  // This is an ArtDmx data packet. It contains zero start code DMX512 information for a single Universe.
#define OpDmx              0x5000  // This is an ArtDmx data packet. It contains zero start code DMX512 information for a single Universe.
#define OpNzs              0x5100  // This is an ArtNzs data packet. It contains non-zero start code (except RDM) DMX512 information for a single Universe.
#define OpAddress          0x6000  // This is an ArtAddress packet. It contains remote programming information for a Node.
#define OpInput            0x7000  // This is an ArtInput packet. It contains enable – disable data for DMX inputs.
#define OpTodRequest       0x8000  // This is an ArtTodRequest packet. It is used to request a Table of Devices (ToD) for RDM discovery.
#define OpTodData          0x8100  // This is an ArtTodData packet. It is used to send a Table of Devices (ToD) for RDM discovery.
#define OpTodControl       0x8200  // This is an ArtTodControl packet. It is used to send RDM discovery control messages.
#define OpRdm              0x8300  // This is an ArtRdm packet. It is used to send all non discovery RDM messages.
#define OpRdmSub           0x8400  // This is an ArtRdmSub packet. It is used to send compressed, RDM Sub-Device data.
#define OpVideoSetup       0xa010  // This is an ArtVideoSetup packet. It contains video screen setup information for nodes that implement the extended video features.
#define OpVideoPalette     0xa020  // This is an ArtVideoPalette packet. It contains colour palette setup information for nodes that implement the extended video features.
#define OpVideoData        0xa040  // This is an ArtVideoData packet. It contains display data for nodes that implement the extended video features.
#define OpMacMaster        0xf000  // This is an ArtMacMaster packet. It is used to program the Node’s MAC address, Oem device type and ESTA manufacturer code. This is for factory initialisation of a Node. It is not to be used by applications.
#define OpMacSlave         0xf100  // This is an ArtMacSlave packet. It is returned by the node to acknowledge receipt of an ArtMacMaster packet.
#define OpFirmwareMaster   0xf200  // This is an ArtFirmwareMaster packet. It is used to upload new firmware or firmware extensions to the Node.
#define OpFirmwareReply    0xf300  // This is an ArtFirmwareReply packet. It is returned by the node to acknowledge receipt of an ArtFirmwareMaster packet or ArtFileTnMaster packet.
#define OpFileTnMaster     0xf400  // Uploads user file to node.
#define OpFileFnMaster     0xf500  // Downloads user file from node.
#define OpFileFnReply      0xf600  // Node acknowledge for downloads.
#define OpIpProg           0xf800  // This is an ArtIpProg packet. It is used to reprogramme the IP, Mask and Port address of the Node.
#define OpIpProgReply      0xf900  // This is an ArtIpProgReply packet. It is returned by the node to acknowledge receipt of an ArtIpProg packet.
#define OpMedia            0x9000  // This is an ArtMedia packet. It is Unicast by a Media Server and acted upon by a Controller.
#define OpMediaPatch       0x9100  // This is an ArtMediaPatch packet. It is Unicast by a Controller and acted upon by a Media Server.
#define OpMediaControl     0x9200  // This is an ArtMediaControl packet. It is Unicast by a Controller and acted upon by a Media Server.
#define OpMediaContrlReply 0x9300  // This is an ArtMediaControlReply packet. It is Unicast by a Media Server and acted upon by a Controller.
#define OpTimeCode         0x9700  // This is an ArtTimeCode packet. It is used to transport time code over the network.
#define OpTimeSync         0x9800  // Used to synchronise real time date and clock
#define OpTrigger          0x9900  // Used to send trigger macros
#define OpDirectory        0x9a00  // Requests a node's file list
#define OpDirectoryReply   0x9b00  // Replies to OpDirectory with file list

#endif
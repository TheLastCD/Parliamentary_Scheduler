# Msg wrapping documents

## Top Level:

```
typedef struct {
  msgHeader hdr;
  msgBody bdy;
  MsgBuffer msg_buff;
} Msg;
```
comprised of 3 main parts:
* header metadata
* body metadata
* payload to be actioned on.

## header
```
typedef struct {
    uint8_t Requester;
    hdr_reqpriority PriorityRequested;
    uint8_t SeqNum;
    uint8_t Localref;
    uint8_t BodyLen;
} msgHeader;

```

The header is comprised of:
* Requester: the ID of the source 
* PriorityRequested: the Priority the source is asking from the scheduler 
* SeqNum: The Local Sequence Number for matching calls
* LocalRef: the Local reference ID, dictates the group the Requester is in
* BodyLen: Length of the payload to be read

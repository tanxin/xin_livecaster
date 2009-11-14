key remoteChannel;
init() {
    llOpenRemoteDataChannel(); // create an XML-RPC channel
    llOwnerSay("My key is " + (string)llGetKey());
}
 
default {
    state_entry() {
        init();
    }
 
    state_exit() {
        return;
    }
 
    on_rez(integer param) {
        llResetScript();        
    }
 
    remote_data(integer type, key channel, key message_id, string sender, integer ival, string sval) {
         if (type == REMOTE_DATA_CHANNEL) { // channel created
             llSay(DEBUG_CHANNEL,"Channel opened for REMOTE_DATA_CHANNEL" + 
                (string)channel + " " + (string)message_id + " " + (string)sender + " " +                         
                (string)ival + " " + (string)sval);
             remoteChannel = channel;
             llOwnerSay("Ready to receive requests on channel \"" + (string)channel + "\"");                        
             state receiving; // start handling requests
         } else {
             llSay(DEBUG_CHANNEL,"Unexpected event type"); 
         }                      
     }                 
}                     
 
 
state receiving {
 
    state_entry() {
        llOwnerSay("Ready to receive information from outside SL");
    }  
 
    state_exit() {
        llOwnerSay("No longer receiving information from outside SL.");
        llCloseRemoteDataChannel(remoteChannel);
    }
 
    on_rez(integer param) {
        //llResetScript();
    }
 
    remote_data(integer type, key channel, key message_id, string sender, integer ival, string sval) {
        if (type == REMOTE_DATA_REQUEST) { // handle requests sent to us
            llParcelMediaCommandList ( [ PARCEL_MEDIA_COMMAND_STOP ] );
            llParcelMediaCommandList([PARCEL_MEDIA_COMMAND_STOP, PARCEL_MEDIA_COMMAND_URL, sval, PARCEL_MEDIA_COMMAND_TYPE, "video/*"]);
            llParcelMediaCommandList ( [ PARCEL_MEDIA_COMMAND_LOOP, PARCEL_MEDIA_COMMAND_PLAY ]);
            llRemoteDataReply(channel,NULL_KEY,"I got it",2008);
            llSay(0,"I got command to play " + sval);
        }
    }
}

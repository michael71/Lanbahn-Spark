// lanbahn2spark.js
// (c) Michael Blank - 2014-2015
// license: GPLv3

// reads multicast lanbahn datagrams and translates them into
// commands for spark signal/turnout decoders
// uses "lanbahn_module.js" for spark http request
// store set messages > 1000 (=MIRROR_LIMIT) and retrieves data if read of 
// already set addresses is requested

// uses translation table "lb2spark.json"

// abbreviations lb=lanbahn
//               dev=spark-device

/*jslint node: true */
/*jslint plusplus: true */
/*jslint bitwise: true */
"use strict";

// constants
var LB_PORT = 27027; // lanbahn port
var LB_GROUP = '239.200.201.250'; // lanbahn multicast group (=address)
var LANBAHN_ERROR = 999; // data value to mark data as ERROR
var MIRROR_LIMIT = 1000; // all data values of addresses >=this value
//will be mirrored (= simulated lanbahn channels)

var DEBUG = true; //log some messages to console, if TRUE

var dgram = require('dgram');
var client = dgram.createSocket('udp4');
var dateFormat = require('dateformat');

var lb2spark = require('./lb2spark.json');
var sparkfunction = require('./lanbahn_module');

// global array for all Lanbahn adr/value pairs we know of
var lanbahndata = [];

// send feedback as UDP message
function sendFeedback(adr, data) {
    var msg = new Buffer("FB " + adr + " " + data);
    client.send(msg, 0, msg.length, LB_PORT, LB_GROUP);
}


function isInteger(value) {
    // see http://stackoverflow.com/questions/14636536/
    //       how-to-check-if-a-variable-is-an-integer-in-javascript
    if (isNaN(value)) {
        return false;
    }
    var x = parseFloat(value);
    return (x | 0) === x;
}

// translate a lanbahn message to a device/cmd-string pair of a spark device
function translate(s) {
    var i;
    for (i = 0; i < lb2spark.length; i++) {
        //console.log(lb2spark[ch]);
        if (lb2spark[i].lb_in === s) {
            return (lb2spark[i]);
        }
    }
    return undefined;
}

// send an initial state 0 for all controlled channels
// HW must start with state=0 for all channels after reset
// save "0" as state for all controlled channels
function init() {
    var i, ch, s, myChannels = [];
    for (i = 0; i < lb2spark.length; i++) {
        s = lb2spark[i].lb_in.split(" ");
        ch = s[1];
        if (myChannels[ch] === undefined) {
            myChannels[ch] = true;
            lanbahndata[ch] = 0;
            sendFeedback(ch, 0);
        }
    }
}


// callback from the spark control function "lanbahn_module.js"
function sparkfunction_callback(err, lb_adr, lb_data) {
    if (err !== null) {
        lb_data = LANBAHN_ERROR;
        // log error message
        var dateshort = dateFormat(Date(), "yyyy-mm-dd h:MM:ss");
        if (DEBUG) {
            console.log(dateshort + ";ERROR: " + err);
        }
    }
    // send feedback message via UDP
    lanbahndata[lb_adr] = lb_data;
    sendFeedback(lb_adr, lb_data);
}


// set client to listen to Lanbahn Multicast Group
client.on('listening', function () {
    var address, dateshort;
    address = client.address();
    // log start message
    dateshort = dateFormat(Date(), "yyyy-mm-dd h:MM:ss ");
    console.log(dateshort + ';lanbahn2spark.js started');
    console.log(dateshort + ';UDP Client listening on ' +
        LB_GROUP + ":" + LB_PORT);
    client.setBroadcast(true);
    client.addMembership(LB_GROUP);
});

// this function is called every time a message is received via UDP
client.on('message', function (message, remote) {
    var str, cmd, t;
    str = message.toString().replace(/\s+/g, ' ').toUpperCase();
    cmd = str.split(' ');
    if (cmd.length < 2) {
        return;
    }
    if (cmd[0] === 'READ') { // reading a lanbahn address is requested
        if (DEBUG) {
            console.log("cmd: " + str);
        }
        if (lanbahndata[cmd[1]] !== undefined) {
            // send only feedback if we have stored the data of this data
            sendFeedback(cmd[1], lanbahndata[cmd[1]]);
        }
    } else if ((cmd[0] === 'SET') && (cmd.length >= 3) && (isInteger(cmd[1])) && (isInteger(cmd[2]))) {
        // a UDP "SET" command is received
        if (DEBUG) {
            console.log("cmd: " + str);
        }
        t = translate(str);
        if (t !== undefined) {
            sparkfunction(t.device, t.spark_cmd, cmd[1], cmd[2], sparkfunction_callback);
        }
        if (isInteger(cmd[1]) && (cmd[1] >= MIRROR_LIMIT)) { // simulate this channel, i.e.
            lanbahndata[cmd[1]] = cmd[2]; // store data
            sendFeedback(cmd[1], lanbahndata[cmd[1]]); // and send a feedback
        }
    } else if ((cmd[0] === 'FB') || (cmd[0] === 'A')) {
        if (DEBUG) {
            console.log("cmd: " + str);
        }
    } else {  // unknown command
        if (DEBUG) {
            console.log("???: " + str);
        }
    }
});

client.bind(LB_PORT);

init();

// lanbahnEvent.js reading the rfid data from the cores
//
// see https://community.spark.io/t/using-spark-publish-with-simple-json-data/3469
//
var EventSource = require('eventsource');
var sparkconfig = require('./sparkconfig.js');

var serverurl = sparkconfig.server_url + '/v1/events/?access_token=' +
         sparkconfig.access_token;
console.log("listening on "+serverurl+" ...");

var source = new EventSource(serverurl);

source.addEventListener("RFID", function (event) {
   //console.log(event.data);
   var rawData = JSON.parse(event.data);
   //var parsedDate = JSON.parse(rawData.data);
   console.log("core="+rawData.coreid+" data="+rawData.data+" t="+rawData.published_at);
   console.log(Date.parse(rawData.published_at));
},false);

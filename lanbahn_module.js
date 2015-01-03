// lanbahn_module.js
// (c) Michael Blank - 2014-2015
// license: GPLv3
//
// calls the "lahnbahnfun" functions of the spark cores
// i.e. the Spark Core must have the proper firmware
//
// input: dev = spark device name
//        cmd = command string for "lanbahnfun" (example "S 0 R")
//        lanbahn_cmd = lanbahn command ok string
//            (which will be given to callback in case of sucess or error)
//        callback(err,data)
// 
/*jslint node: true */
/*jslint plusplus: true */
"use strict";

var request = require('request');
var sparkconfig = require('./sparkconfig.js');

module.exports = function lanbahnfunction(dev, cmd, lb_adr, lb_data, callback) {
    var serverurl = sparkconfig.server_url + '/v1/devices/' + dev + '/lanbahnfun',
        token = sparkconfig.access_token;
    request.post({
        url: serverurl,
        form: {
            args: cmd,
            access_token: token
        },
        timeout: 300
    },
        function (error, response, body) {
            var res, ret_value;
            if (!error && response.statusCode === 200) {
                res = JSON.parse(body);
                ret_value = res.return_value;
                if (ret_value > 0) {
                    callback(null, lb_adr, lb_data);
                } else {
                    callback("Failure r=" + ret_value, lb_adr, lb_data);
                }
            } else {
                callback(error, lb_adr, lb_data);
            }
        }
        );
};

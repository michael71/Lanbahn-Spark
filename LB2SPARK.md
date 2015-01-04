<h1>LB2SPARK Howto</h1>

<p>The data in the json file "lb2spark.json2" define the mappings between the Lanbahn commands (like "SET 760 1" - set the accessory on address 760 to a value of 1) and the commands which are sent to the spark devices (like send command "S 0 G" to the lanbahn device "lanbahn3").</p>

<pre>example - definitions for LANBAHN address 760:

[  {    "lb_in": "SET 760 0",    "device": "lanbahn3",    "spark_cmd": "S 0 R"  },
   {    "lb_in": "SET 760 1",    "device": "lanbahn3",    "spark_cmd": "S 0 G"  },
   {    "lb_in": "SET 760 2",    "device": "lanbahn3",    "spark_cmd": "S 0 Y"  },
   {    "lb_in": "SET 760 3",    "device": "lanbahn3",    "spark_cmd": "S 0 F"  }
]
</pre>

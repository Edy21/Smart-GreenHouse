const url = require("url");
const http = require("http");
const requestClient = require("request-promise");
const fs = require('fs');
const moment = require('moment');

let sera = ['192.168.1.177', '192.168.1.2', '192.168.1.3']

var luminozitate = [];
var temperatura = [];
var umiditateAer= [];
var umiditateSol = [];

const app = http.createServer((request, response) => {

  response.setHeader('Access-Control-Allow-Origin', '*');
  response.setHeader('Access-Control-Request-Method', '*');
  response.setHeader('Access-Control-Allow-Methods', 'OPTIONS, GET');
  response.setHeader('Access-Control-Allow-Headers', '*');

  let query = url.parse(request.url, true).query;
  let path = url.parse(request.url, true).pathname;
  if(path === '/logs'){
	fs.readFile('./values.txt', (error, data) => {
          response.end(data.toString());
        });	
  }else if(path === '/graph'){
        response.end(JSON.stringify({
             luminozitate,
             temperatura	
       }));
  }else{
          
	  requestClient("http://" + sera[query.sera-1] + "?action="+query.action +"&durata="+query.durata +"&lum=" +query.lum + "&temp=" + query.temp + "&uma=" + query.uma + "&ums=" + query.ums).then(body => {
	      

	      if(query.action == 3 || query.action == 4 || query.action == 6){

		      switch(Number(query.action)){

		         case 3:
			    luminozitate.push({
				x: new Date(),
				y: parseInt(body)
			    });
			 break;

			case 4:
                            temperatura.push({
                                x: new Date(),
                                y: parseInt(body)
                            });
                        break;
		      }
	  
		      let text = new moment().format("DD/MM/YYYY HH:mm") + ' - '+ query.sera + ' - ' + query.numeActiune + ' : ' + body + '\n';
		      
		      
	      fs.appendFile('./values.txt', text, function(err) { console.log(err) }	);
	      }
	      response.end(body ? body: null)

	  })
  }


}); 

app.listen(3000);

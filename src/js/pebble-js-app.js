
function getLocation() {
	navigator.geolocation.getCurrentPosition(

		function(pos){

			var xhr = new XMLHttpRequest();
			var url = 'http://swu_gtfs.hannenz.de?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
			console.log(url);
			xhr.open('GET', url);
			xhr.onload = function(response){
				if (xhr.readyState == 4 && xhr.status == 200){

					console.log(this.responseText);

					var data = JSON.parse(this.responseText);

					for (var i = 0; i < data.length; i++){
						var dict = {
							0 : '',
							1 : data[i]['route_short_name'],
							2 : data[i]['route_long_name'],
							3 : data[i]['display_name'],
							4 : data[i]['departure_timestamp']
						};
						Pebble.sendAppMessage(
							dict,
							function(e) { console.log('Send successful.')},
							function(e) { console.log('Send failed.')}
						);
					}

				}
			};
			xhr.send(null);
		},
		function(err){
			console.log('getCurrentPosition() failed');
			Pebble.sendAppMessage({
				0 : 'Failed to get current position'
			});
		},
		{
			timeout : 15000,
			maximumAge : 60000
		}
	);
}

Pebble.addEventListener("ready",
    function(e) {
        console.log("Hello world! - Sent from your javascript application.");
	    getLocation();
    }
);

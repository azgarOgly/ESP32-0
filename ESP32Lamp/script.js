function up() {
	const L0input = document.getElementById('L0input');
	const L1input = document.getElementById('L1input');
	const L2input = document.getElementById('L2input');
	
	const Http = new XMLHttpRequest();
	const url='data?L0=' + L0input.value + '&L1=' + L1input.value  + '&L2=' + L2input.value;
	Http.open('GET', url);
	Http.send();

	Http.onreadystatechange = (e) => {
	  // console.log(Http.responseText)
	} 
}

function renewForm() {
	const Http = new XMLHttpRequest();
	const url='data';
	Http.open('GET', url);
	Http.send();

	Http.onreadystatechange = (e) => {
	  const rows = Http.responseText.split('\n');
	  rows.forEach(r => {
		  const row = r.split('=');
		  assignValue(row[0], row[1]);
	  });
	} 
}

function assignValue(key, value) {
	const L0input = document.getElementById('L0input');
	const L1input = document.getElementById('L1input');
	const L2input = document.getElementById('L2input');
	
	if (key === 'L0') {
		L0input.value = value;
	} else if (key === 'L1') {
		L1input.value = value;
	} else if (key === 'L2') {
		L2input.value = value;
	}
}
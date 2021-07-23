async function parseList() {
	const list = await getList();
	console.log(list);
}

async function getList() {
	const url="/list"
	const response = await fetch(url);
	return response.text();
}
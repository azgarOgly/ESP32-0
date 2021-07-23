async function displayList() {
	document.getElementById("list").innerHTML = "";
	const list = await parseList();
	list.forEach(e => {
		document.getElementById("list").innerHTML += "<p><a href='" + e.name + "'> " + e.name + "</a> " + e.size + "</p>";
	});
}

async function parseList() {
	const list = await getList();
	const items = list.split("\n");
	const formatted = 
		items.
			filter(i => i !== "Listing files: ").
			map(i => {
				if (i.startsWith("/")) {
					i = i.replace("/", "")
				}
				fields = i.split(": ");
				return {
					name: fields[0],
					size: fields[1]
				};
			});
	return formatted;
}

async function getList() {
	const url="/list"
	const response = await fetch(url);
	return response.text();
}
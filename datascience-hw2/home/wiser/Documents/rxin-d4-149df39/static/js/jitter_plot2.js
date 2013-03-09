d3.csv("preprocessed-data/statedon.csv", function handleCSV(csv) {
  // select revenues of CA companies
  var data = csv.filter(function(el) { 
                          return el.state != ""; })
                .map(function(el) { 
                          return parseInt(el.amount) });

  var data2 = csv.filter(function(el) { 
      return el.state != ""; });


  var w = 235,
  h = 185,
  p = 20,
  r = 3,
  x = d3.scale.linear()
              .domain([0, d3.max(data) * 1.01])
              .range([0, w]),
  y = d3.scale.ordinal()
              .domain(["State"])
              .range([h/2]);

var vis = d3.select("div #jitter")
          .append("svg")
          .attr("width", w + p * 2)
          .attr("height", h + p * 2)
          .append("g")
          .attr("transform", "translate(" + p + "," + p + ")");

// x axis
var xrule = vis.selectAll("g.x")
             .data(x.ticks(5))
             .enter()
             .append("g")
             .attr("class", "x");

xrule.append("line")
   .attr("x1", x)
   .attr("x2", x)
   .attr("y1", 0)
   .attr("y2", h);

xrule.append("text")
   .attr("x", x)
   .attr("y", h + 3)
   .attr("dy", ".71em")
   .attr("text-anchor", "middle")
   .text(x.tickFormat(5));

xrule.append("text")
.attr("x", w/2)
.attr("text-anchor", "middle")
.attr("y", h+20)
.text("Number of Donations");

// y axis
var yrule = vis.selectAll("g.y")
             .data(["State"])
             .enter()
             .append("g")
             .attr("class", "y");

yrule.append("line")
   .attr("x1", -3)
   .attr("x2", 0)
   .attr("y1", y)
   .attr("y2", y);

yrule.append("text")
   .attr("x", 10)
   .attr("y", y+.2)
   .attr("dy", ".35em")
   .attr("text-anchor", "end")
   .text(String);

// draw the dots
vis.selectAll("circle")
 .data(data)
 .enter()
 .append("circle")
 .attr("cx", x)
 .attr("cy", function() { return (h/10) + (Math.random() * (9*h/10)) })
 .attr("r", r)
 .on("mouseover", function(d, i){d3.select("div#jitter g.x").append("text").attr("id", "tip").attr("x", x(d)+5).attr("y", d3.select(this).attr("cy")-5).attr("text-anchor", "start").attr("font-size", "8")
	 .text(function(s){ return data2[i].state;}); d3.select(this).attr("fill", "red")})
 .on("mouseout", function(d, i){d3.select("g.x #tip").remove(); d3.select(this).attr("fill", "black")});
});




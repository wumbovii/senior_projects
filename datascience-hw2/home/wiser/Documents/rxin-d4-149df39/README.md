# CS194-16 Data Science Assignment 2 - Data Visualization

In this assignment, you will construct a simple visualization dashboard for the presidential campaign finance data used in the previous assignment. We have created a skeleton dashboard for you, and you will need to implement the JavaScript and data preprocessing yourself.

Your visualizations should be designed to inform viewers about the presidential race. Each visualization should be designed to *teach* the viewer something; this lesson should be presented with appropriate use of visualization techniques. You might choose to visualize various combinations of fields and dimensions from the data set: for instance, you might present data about a single candidate in the data set, all the donations within a certain state, or information about all the candidates. Although not required, you can also use external data sets and combine them with this data set to provide more information.

You can look at http://elections.nytimes.com/2012/campaign-finance for examples of such visualizations. However, do not simply copy their visualization.  


##Setup

We have created a skeleton dashboard with six visualizations using d3 for the Fortune 500 dataset and put it on GitHub at https://github.com/rxin/d4/

If you have git installed on your computer, do 
``git@github.com:rxin/d4.git``
to clone the repository.

If you don’t have git, you can download the code at https://github.com/rxin/d4/downloads


##Implementation and Testing

Read index.html and the JavaScript files in d4/static to understand how the plots are created and then implement six different visualizations of your choice using the skeleton code.

You are strongly encouraged to use a modern browser to develop this dashboard for better performance and development tools. The GSI (Reynold) will use the latest beta version (18.0) of Google Chrome to grade your submission.

Since most browsers block the ability for JavaScript to load local disk files, you should test your dashboard in a web server. The simplest way is to use the following Python one-liner in your d4 dashboard folder:

```python -m SimpleHTTPServer 8000```

This starts a HTTP server on port 8000 serving all files in the current folder. You will then be able to view the dashboard at http://localhost:8000/index.html

Note that in the provided dashboard, the entire f500.csv is read by the client side JavaScript. This is fine since f500.csv is a small file. In the case of presidential campaign data, P00000001-ALL.txt is very large and you will have a hard time processing that file using client-side JavaScripts.

You should think about the visualizations you’d like to implement and preprocess the raw data using pandas and generate small, summary csv files before loading them in d3. You can create multiple preprocessed files for different visualizations.


##Writeup

You should include a short writeup describing your thought process. The writeup should include:

- What kind of data pre-processing have you done on the server side? 

- What transformations on the data do the client-side d3 apply to generate the plots? 

- What is the intent of each visualization? i.e., what information do you want to present to the viewer?

- What are the right visualization techniques to present this information effectively? (e.g., which type of chart or graph is used? why?)


##Submission

You should submit the d4 folder in a compressed (zip) file, using the following directory structure:

<pre>

d4
 |-- data: All external data files. Do NOT include P00000001-ALL.txt
 |-- preprocessed-data: The preprocessed data files.
 |-- static:
 |   |-- css: CSS files.
 |   |-- img: Any image files you need.
 |   |-- js: The JavaScript files to generate the plots.
 |-- index.html: The dashboard html file.
 |-- report.pdf: A short report in PDF format.
 |-- data_preprocess: The script to preprocess data.

</pre>

Once unzipped, the GSI should be able to see the dashboard by running the Python SimpleHTTPServer in the d4 folder.

Limit the report to two pages. It is fine if it is much shorter as long as you can sufficiently describe your thought process. You will be penalized for excessively verbose answers. Include your name and email in the report file.

Have fun!

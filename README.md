# Cash-Register-using-Client-Server
It is a simple demo of cash registry using client server, where the client request server price of products and the server response with the price. Error handeling is also taken into consideration.

<br>Command to run the server: **./server IP PORT** ## server does not required IP as INADDR_ANY is used.
<br>Command to run the client: **./client ServerIP ServerPORT**

The client sends the request and the server gives the reply.
<br>Request format: **Type Product_UID Product_QTY**
                <br>Type 0: then client requests the price of the product.
                <br>Type 1: then client requests for closing the connection.
<br>Response format: **Type Total_Price Product_Name**
                <br>Type 0: it is a valid response.
                <br>Else: The response is invalid.
<br>Before closing the connection the server, after receiving Type 1, sends the total price to the client and closes the client socket.
<br>Concurrent client access is implemented using **fork()** system call.

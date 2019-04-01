<%-- 
    Document   : hello
    Created on : 2019-03-12, 09:57:52
    Author     : dstudent
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<%
    request.setCharacterEncoding("UTF-8");
%>
<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>JSP Page</title>
    </head>
    <body>
        <%
        String imie = request.getParameter("var1"); 
        out.println("Witaj " + imie);
        %>
        <p> Podaj liczby wierszyków: </p>

        Liczba wierszy: <input type="text" name="var1"/> <br/>
        Liczba kolumn: <input type="text" name="var2"/> <br/>
        <form action="table.jsp">
        <input type="submit" name=typ value="Mnozenia" />
        <input type="submit" name=typ value="Slimak" />
        </form>
                       
                

    </body>
</html>

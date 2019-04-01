<%-- 
    Document   : table
    Created on : 2019-03-12, 09:57:25
    Author     : dstudent
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>JSP Page</title>
    </head>
    <body>
        <%
        Integer wierszy = Integer.parseInt(request.getParameter("var1"));
        Integer kolumn = Integer.parseInt(request.getParameter("var2"));
        String type = request.getParameter("typ");
             
        int[][] values = new int[wierszy+1][kolumn+1];
        
        if(type.matches("Mnozenia")){
             for(int i =1; i<=wierszy; i++){
                for(int j =1; j<=kolumn; j++){
                    values[i][j] = i*j;
                    }
                }
        }
        else{
            int currentCols = kolumn;
            int currentRows = wierszy;
            int row = 1;
            int col =1;
            for( col = 1; col<=currentCols; col++){
                values[i][] = currentIndex++;
            }    
            for( row =1; row<=currentRows; row++){
                    
                    
            }
          }
        %>
                <table style="width:100%">
                <%for(int i =1; i<=wierszy; i++){%>
                <tr>
                <%for(int j =1; j<=kolumn; j++){%>
                    <th><%=values[i][j]%></th>
                    <% } %>
                </tr>
                <% } %>
                </table>
    </body>
</html>

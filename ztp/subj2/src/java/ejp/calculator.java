/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ejp;

import javax.ejb.Stateless;

/**
 *
 * @author Student
 */
@Stateless
public class calculator {

    public double handleOperation(String operation, double var1, double var2)
    {
        switch(operation){
            case "/": return var1/var2;
            case "*": return var1*var2;
            case "+": return var1+var2;
            case "-": return var1-var2;
            case "SQRT" : return Math.sqrt(var1*var1+var2*var2);
            default : return 0xFFFFFFFF;
        }
     
    
    }
    // Add business logic below. (Right-click in editor and choose
    // "Insert Code > Add Business Method")
}

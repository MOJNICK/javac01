/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ejb;

import java.io.Serializable;
import java.util.Random;
import javax.inject.Named;
import javax.enterprise.context.SessionScoped;
import javax.faces.context.FacesContext;

@Named(value = "test")
@SessionScoped
public class test implements Serializable{

    private int usedChanses; 

    public int getUsedChanses() {
        return usedChanses;
    }

    public void setUsedChanses(int usedChanses) {
        this.usedChanses = usedChanses;
    }
    private int randomNumber;

    public int getRandomNumber() {
        return randomNumber;
    }

    public void setRandomNumber(int randomNumber) {
        this.randomNumber = randomNumber;
    }
    
    
    private int Ax;

    public void setAx(int Ax) {
        this.Ax = Ax;
        usedChanses++;
    }

    public int getAx() {
        return Ax;
    }
    
    public int matchWithRandom(){
        if(Ax > randomNumber){
                return 1;
        }
        else
        {
            if(Ax < randomNumber){
                return -1;
            }
            else{
                return 0;
            }
        }
    }
    
    public void invalidateSession(){
    FacesContext.getCurrentInstance().getExternalContext().invalidateSession();
        //return "/index.xhtml?faces-reditect=true";
    }
    
    /**
     * Creates a new instance of test
     */
    public test() {
        Random rand = new Random();
        randomNumber = rand.nextInt(14)+1;
        usedChanses = 0;
    }
}

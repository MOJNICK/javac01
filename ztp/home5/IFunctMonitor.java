package pl.jrj.fnc;

import javax.ejb.Remote;

/**
 *
 * @author Szymon Szozda
 */
@Remote
public interface IFunctMonitor {
    /**
    * @param x, y
    * @return returns function value at (x,y) point
    */
    public double f( double x, double y );   
}

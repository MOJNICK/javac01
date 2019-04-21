import javax.naming.InitialContext;

import java.util.ArrayList;
import java.util.Random;
import javax.ejb.Stateless;
import javax.naming.NamingException;
import pl.jrj.fnc.IFunctMonitor;

/**
 * This class has solve() method which is used to calculate integral,
 * @author Szymon Szozda
 */
@Stateless
public class IBean implements IBeanRemote {
    
    /**
    *
    * @author Szymon Szozda
    * @return Calculated integral value.
    * @param a is y interval: [0,a]. b is x interval: [0,b]
    */
    @Override
    public double solve(double a, double b, int n) {
        double retval = 0.0;
                        IntegralCalc ic = new IntegralCalc();
                        retval = ic.calcWithPrecision(Math.pow(10, -n)
                                , a, b);
        return retval;
    }
    
    /**
     * MonteCarlo algorithm implementation.
     * @author Szymon Szozda
     */
    static class MonteCarlo{
        Compute compute;
            
        MonteCarlo(Compute compute) {
            this.compute = compute;
        }
        
        private double run(int iterations, double a, double b){
            double agregationValue = 0.0;
            for (int i=0; i< iterations; ++i){
                agregationValue += compute.value(a, b);
            }
            return agregationValue / iterations;//avg value
        }
    }
    
     /**
      * This class runs Monte Carlo until desired precision.
      *
      * @author Szymon Szozda
      * @version 0.01
      */
    static class IntegralCalc{
        private double calcIntegral(int iterations, double a, double b){
            Compute c = new Compute();
            MonteCarlo mc = new MonteCarlo(c);
            double integralValue = mc.run(iterations, a, b);
            integralValue = integralValue * a * b;
            return integralValue;
        }
        
        private double mean(ArrayList<Double> values){
            double sum = 0.0;
            for (double val : values)
            {
                sum+=val;
            }
            double mean = sum / values.size();
            return mean;
        }
        
        private double calcSigma(ArrayList<Double> weights){
            double mean = mean(weights);
            ArrayList<Double> squareDiffs = new ArrayList<Double>();
            for (double weight : weights){
                squareDiffs.add(Point.square(mean-weight));
            }
            double sigma = Math.sqrt(mean(squareDiffs));
            return sigma;
        }
        
        private double calcWithPrecision(double absolutePrecision,
        double a, double b){
            ArrayList<Double> calculatedValues = new ArrayList<Double>();
            final int runs = 10;
            int iterations = 1000;
            final int iterationsMultiplier = runs;
            
            for (int i = 0; i < runs; ++i)
            {
                double weight = calcIntegral(iterations, a, b);
                calculatedValues.add(weight);
            }
            double averageWeight = mean(calculatedValues);
            double sigma = calcSigma(calculatedValues);
                    
            while (sigma > absolutePrecision){
                calculatedValues.clear();
                calculatedValues.add(averageWeight);
                iterations*=iterationsMultiplier;
                for (int i = 1; i < runs; ++i)
                {
                    double weight = calcIntegral(iterations, a, b);
                    calculatedValues.add(weight);
                }
                averageWeight = mean(calculatedValues);
                sigma = calcSigma(calculatedValues);
            }
            
            return averageWeight;
        }      
    }
    
    static class Point{
        double x;
        double y;
        static Random random = new Random(System.currentTimeMillis());
        
        public static double square(double x){
            return x*x;
        }
        
        public void randomizeCoordsForSquare(double a, double b){
            x = random.nextDouble() * b;
            y = random.nextDouble() * a;
        }
    }
        
   /**
    * Implementation of Compute
    *
    * @author Szymon Szozda
    * @version 0.01
    */
    static class Compute{
        private Point p;
        private IFunctMonitor myFunct = null;
        
        Compute() {
            this.p = new Point();
            try {
		InitialContext context = new InitialContext();
		myFunct = (IFunctMonitor) context.lookup(
                        "java:global/ejb-project/FunctMonitor!pl.jrj.fnc.IFunctMonitor");

	    } catch (NamingException e) {
                //print error TODO print problem TO DO TO_DO
	    }
        }
        
   /**
    *
    * @return Value for montecarlo algorithm.
    */
    public double value(double a, double b){
            double val = 0.0;
            p.randomizeCoordsForSquare(a, b);
            val = myFunct.f(p.x, p.y);
            return val;
        }
    }
    
}

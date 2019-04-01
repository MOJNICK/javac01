/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Main service calass. Handles GET and POST.
 *
 * @author Szymon Szozda
 * @version 0.01
 */
public class Ellipsoid extends HttpServlet {
    Data data = new Data();

   /**
    *
    * @author Szymon Szozda
    * @version 0.1
    */
    static class Point{
        static Random random = new Random(System.currentTimeMillis());
        double coordX;
        double coordY;
        double coordZ;
        
        Point(double x, double y, double z){      
            this.coordX = x;
            this.coordY = y;
            this.coordZ = z;
        }
        
        boolean isInsideElipsoid(Elip elipsoid){
            return square(coordX)/square(elipsoid.semiAxleAX)
                 + square(coordY)/square(elipsoid.semiAxleBY)
                 + square(coordZ)/square(elipsoid.semiAxleCZ)
                    <= 1;
        }
        
        boolean isInsideSphere(Sphere sphere){
            return square(coordX - sphere.center.coordX)
            + square(coordY - sphere.center.coordY)
            + square(coordZ - sphere.center.coordZ)
            <= square(sphere.radious);
        }
        
       /**
        * Calc square.
        *
        * @param number to square.
        * @return result.
        */
        public static double square(double x){
            return x*x;
        }
        
        private static double normalizeInterval(double value, double srcMin,
        double srcMax, double dstMin, double dstMax){
            value *= (dstMax - dstMin) / (srcMax - srcMin);
            value += (dstMin - srcMin);
            return value;
        }
        
        private void randomizeCoords(double xMin, double xMax, double yMin,
        double yMax, double zMin, double zMax){
            coordX = normalizeInterval(random.nextDouble(), 0, 1.0, xMin, xMax);
            coordY = normalizeInterval(random.nextDouble(), 0, 1.0, yMin, yMax);
            coordZ = normalizeInterval(random.nextDouble(), 0, 1.0, zMin, zMax);
        }
        
        void randomizeCoordsForElipsis(double semiAxleX, double semiAxleY,
        double semiAxleZ){
            randomizeCoords(-semiAxleX, semiAxleX, -semiAxleY, semiAxleY,
            -semiAxleZ, semiAxleZ);
        }
    }
    
   /**
    * Sphere representation.
    *
    * @author Szymon Szozda
    * @version 0.01
    */
    static class Sphere{
        static double ro = -100000;
        Point center;
        double radious;
    
        Sphere(Point center, double radious) {
            this.center = center;
            this.radious = radious;
        }
    }
    
   /**
    * Elipsis representation.
    *
    * @author Szymon Szozda
    * @version 0.01
    */
    static class Elip{
        double semiAxleAX;
        double semiAxleBY;
        double semiAxleCZ;
        double ro;
    
        Elip(double a, double b, double c, double ro) {
            this.semiAxleAX = a;
            this.semiAxleBY = b;
            this.semiAxleCZ = c;
            this.ro = ro;
        }
    }
    
   /**
    * Interface for different algorithms.
    *
    * @author Szymon Szozda
    * @version 0.01
    */
    interface Compute{
        double value();
    }
    
   /**
    * Implementation of Compute interface
    *
    * @author Szymon Szozda
    * @version 0.01
    */
    static class DefectDoNotChangeElipShape implements Compute{
        ArrayList<Sphere> spheres;
        Elip elipsoid;
        private Point p;
    
        DefectDoNotChangeElipShape(Data data) {
            this.spheres = data.spheres;
            this.elipsoid = data.elipsoid;
            this.p = new Point(0.0, 0.0, 0.0);
        }
        
   /**
    * Sphere representation.
    *
    * @return Value for montecarlo algo.
    */
    @Override
    public double value(){//returns point ro
            double ro = 0.0;
            p.randomizeCoordsForElipsis(elipsoid.semiAxleAX, elipsoid.semiAxleBY,
            elipsoid.semiAxleCZ);
            if (p.isInsideElipsoid(elipsoid)){
                ro = elipsoid.ro;
                for (Sphere sphere : spheres){
                    if (p.isInsideSphere(sphere)){
                        ro = Sphere.ro;
                        break;
                    }
                }
            }
            return ro;
        }
    }
        
   /**
    * Container for spheres and elipsoid.
    *
    * @author Szymon Szozda
    * @version 0.01
    */
    static class Data{
        ArrayList<Sphere> spheres;
        Elip elipsoid;
    
        Data() {
            this.spheres = new ArrayList<Sphere>();
        }
        
    }
        
        /**
         * Monte Carlo algorithm.
         *
         * @author Szymon Szozda
         * @version 0.01
         */
    static class MonteCarlo{
        Compute compute;
            
        MonteCarlo(Compute compute) {
            this.compute = compute;
        }
        
        double run(int iterations){
            double agregationValue = 0.0;
            for (int i=0; i< iterations; ++i){
                agregationValue += compute.value();
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
    static class WeightCalc{
        private double calcWeight(Data data, int iterations){
            Compute c = new DefectDoNotChangeElipShape(data);
            MonteCarlo mc = new MonteCarlo(c);
            double medianRo = mc.run(iterations);
            double cuboidVolume = 8
            * data.elipsoid.semiAxleAX
            * data.elipsoid.semiAxleBY
            * data.elipsoid.semiAxleCZ;
            double weight = cuboidVolume * medianRo;
            return weight;
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
        
        double calcWithPrecision(Data data, double absolutePrecision){
            ArrayList<Double> weights = new ArrayList<Double>();
            final int runs = 10;
            int iterations = 1000;
            final int iterationsMultiplier = runs;
            
            for (int i = 0; i < runs; ++i)
            {
                double weight = calcWeight(data, iterations);
                weights.add(weight);
            }
            double averageWeight = mean(weights);
            double sigma = calcSigma(weights);
                    
            while (sigma > absolutePrecision){
                weights.clear();
                weights.add(averageWeight);
                iterations*=iterationsMultiplier;
                for (int i = 1; i < runs; ++i)
                {
                    double weight = calcWeight(data, iterations);
                    weights.add(weight);
                }
                averageWeight = mean(weights);
                sigma = calcSigma(weights);
            }
            
            return averageWeight;
        }
        
    }
    // <editor-fold defaultstate="collapsed" desc="HttpServlet methods.
    // Click on the + sign on the left to edit the code.">
    /**
     * Handles the HTTP <code>GET</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest request,
    HttpServletResponse response)
            throws ServletException, IOException {
        double x = Double.parseDouble(request.getParameter("x"));
        double y = Double.parseDouble(request.getParameter("y"));
        double z = Double.parseDouble(request.getParameter("z"));
        double r = Double.parseDouble(request.getParameter("r"));
        
        data.spheres.add(new Sphere(new Point(x, y, z), r));
    }

    /**
     * Handles the HTTP <code>POST</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest request,
    HttpServletResponse response)
            throws ServletException, IOException {
        double a = Double.parseDouble(request.getParameter("a"));
        double b = Double.parseDouble(request.getParameter("b"));
        double c = Double.parseDouble(request.getParameter("c"));
        double v = Double.parseDouble(request.getParameter("v"));
        double g = Double.parseDouble(request.getParameter("g"));
                
        data.elipsoid = new Elip(a, b, c, v);
        Sphere.ro = g;
        
        
        double absolutePrecision = 0.05;//with sigma accuracy
        WeightCalc wc = new WeightCalc();
        double weight = wc.calcWithPrecision(data, absolutePrecision);
        weight = Math.round(weight * 100.0) / 100.0;
        String strDouble = String.format("%.2f", weight);
        response.getWriter().print(strDouble);
        
        data.spheres = new ArrayList<Sphere>();
    }

    /**
     * Returns a short description of the servlet.
     *
     * @return a String containing servlet description
     */
    @Override
    public String getServletInfo() {
        return "Short description";
    }// </editor-fold>

}

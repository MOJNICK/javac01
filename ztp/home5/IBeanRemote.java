import javax.ejb.Remote;

/**
 *
 * @author Szymon Szozda
 */
@Remote
public interface IBeanRemote {
	public double solve(double a, double b, int n);
}

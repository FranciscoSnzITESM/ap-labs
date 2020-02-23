public class FirstChallenge {
	public static void main(String[] args) {
		String str = args[0];
		int highest, current;
		highest = current = 1;
		String cStr = str.substring(0, 1);
		for(int i=1; i<str.length(); i++) {
			String ch = str.substring(i,i+1);
			if(!cStr.contains(ch)) {
				cStr = cStr.concat(ch);
				current++;
			}
			else {
				if(current > highest) {
					highest = current;
				}
				cStr = ch;
				current = 1;
			}
		}
		System.out.println(highest);
	}
}

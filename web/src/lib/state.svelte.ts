export class Selection {
    public selectedBox3: number | null = $state(null);
    public selectedBox2: number | null = $state(null);

    public selectBox3(index: number) {
        this.selectedBox2 = null;
        this.selectedBox3 = index;
    }

    public selectBox2(index: number) {
        if (this.selectedBox3 === null) {
            return;
        }
        this.selectedBox2 = index;
    }
}
